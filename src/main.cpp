
#include <NimBLEDevice.h>
#include "Arduino.h"
#include <BLEMacros.h>
#include <FileManager.h>
#include <DataLogging.h>
#include <STATES_MACROS.h>


static volatile bool print_on = false;
static volatile bool xperiment_on = false;
static volatile bool restart_on = false;
bool were_scanning_print = false;




void scanEndedCB(NimBLEScanResults results);

static NimBLEAdvertisedDevice* advDevice;

static bool doConnect = false;
static uint32_t scanTime = 0; /** 0 = scan forever */


static int SWC_counter = 0;
static int timestep_count = 0;

static unsigned char * SWC_read = nullptr;
static unsigned long time_last_read = 0L;
unsigned long time_start = millis();

static int state = STATE_IDLE;



void IRAM_ATTR printon_interrupt()
{
    print_on = !print_on;
}

void IRAM_ATTR xperimenton_interrupt()
{
    xperiment_on = !xperiment_on;
    state = STATE_IDLE;
}

void IRAM_ATTR restart_interrupt()
{
    restart_on = true;
}

NimBLEClient * pClient_SWC;
SPIFFSFileManager& fileMane = SPIFFSFileManager::get_instance();


/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) {
        Serial.println("Connected");
        /** After connection we should change the parameters if we don't need fast response times.
         *  These settings are 150ms interval, 0 latency, 450ms timout.
         *  Timeout should be a multiple of the interval, minimum is 100ms.
         *  I find a multiple of 3-5 * the interval works best for quick response/reconnect.
         *  Min interval: 120 * 1.25ms = 150, Max interval: 120 * 1.25ms = 150, 0 latency, 60 * 10ms = 600ms timeout
         */
        pClient->updateConnParams(120,120,0,60);
    };

    void onDisconnect(NimBLEClient* pClient) {
        Serial.print(pClient->getPeerAddress().toString().c_str());
        Serial.println(" Disconnected ");
        pClient->disconnect();
        NimBLEDevice::getScan()->start(scanTime, scanEndedCB);
    };

    /** Called when the peripheral requests a change to the connection parameters.
     *  Return true to accept and apply them or false to reject and keep
     *  the currently used parameters. Default will return true.
     */
    bool onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params) {
        if(params->itvl_min < 24) { /** 1.25ms units */
            return false;
        } else if(params->itvl_max > 40) { /** 1.25ms units */
            return false;
        } else if(params->latency > 2) { /** Number of intervals allowed to skip */
            return false;
        } else if(params->supervision_timeout > 100) { /** 10ms units */
            return false;
        }

        return true;
    };

    /********************* Security handled here **********************
    ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest(){
        Serial.println("Client Passkey Request");
        /** return the passkey to send to the server */
        return 123456;
    };

    bool onConfirmPIN(uint32_t pass_key){
        Serial.print("The passkey YES/NO number: ");
        Serial.println(pass_key);
        /** Return false if passkeys don't match. */
        return true;
    };

    /** Pairing process complete, we can check the results in ble_gap_conn_desc */
    void onAuthenticationComplete(ble_gap_conn_desc* desc){
        if(!desc->sec_state.encrypted) {
            Serial.println("Encrypt connection failed - disconnecting");
            /** Find the client with the connection handle provided in desc */
            NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
            return;
        }
    };
};


/** Define a class to handle the callbacks when advertisments are received */
class AdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {

    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        if(advertisedDevice->isAdvertisingService(NimBLEUUID(SWC_SRVI_UUID)))
        {
            Serial.println("Found Our Service");
            /** stop scan before connecting */
            NimBLEDevice::getScan()->stop();
            /** Save the device reference in a global for the client to use*/
            advDevice = advertisedDevice;

            /** Ready to connect now */
            doConnect = true;
        }
    };
};


/** Notification / Indication receiving handler callback */
void notifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify){
    std::string str = (isNotify == true) ? "Notification" : "Indication";
    str += " from ";
    /** NimBLEAddress and NimBLEUUID have std::string operators */
    str += std::string(pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress());
    str += ": Service = " + std::string(pRemoteCharacteristic->getRemoteService()->getUUID());
    str += ", Characteristic = " + std::string(pRemoteCharacteristic->getUUID());
    str += ", Value = " + std::string((char*)pData, length);
    Serial.println(str.c_str());
}

/** Callback to process the results of the last scan or restart it */
void scanEndedCB(NimBLEScanResults results){
    Serial.println("Scan Ended");
}


/** Create a single global instance of the callback class to be used by all clients */
static ClientCallbacks clientCB;


/** Handles the provisioning of clients and connects / interfaces with the server */
bool connectToServer() {
    NimBLEClient* pClient = nullptr;

    /** Check if we have a client we should reuse first **/
    if(NimBLEDevice::getClientListSize()) {
        /** Special case when we already know this device, we send false as the
         *  second argument in connect() to prevent refreshing the service database.
         *  This saves considerable time and power.
         */
        pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
        if(pClient){
            if(!pClient->connect(advDevice, false)) {
                Serial.println("Reconnect failed");
                return false;
            }
            Serial.println("Reconnected client");
        }
            /** We don't already have a client that knows this device,
             *  we will check for a client that is disconnected that we can use.
             */
        else {
            pClient = NimBLEDevice::getDisconnectedClient();
        }
    }

    /** No client to reuse? Create a new one. */
    if(!pClient) {
        if(NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS) {
            Serial.println("Max clients reached - no more connections available");
            return false;
        }

        pClient = NimBLEDevice::createClient();

        Serial.println("New client created");

        pClient->setClientCallbacks(&clientCB, false);
        /** Set initial connection parameters: These settings are 15ms interval, 0 latency, 120ms timout.
         *  These settings are safe for 3 clients to connect reliably, can go faster if you have less
         *  connections. Timeout should be a multiple of the interval, minimum is 100ms.
         *  Min interval: 12 * 1.25ms = 15, Max interval: 12 * 1.25ms = 15, 0 latency, 51 * 10ms = 510ms timeout
         */
        pClient->setConnectionParams(12,12,0,51);
        /** Set how long we are willing to wait for the connection to complete (seconds), default is 30. */
        pClient->setConnectTimeout(5);


        if (!pClient->connect(advDevice)) {
            /** Created a client but failed to connect, don't need to keep it as it has no data */
            NimBLEDevice::deleteClient(pClient);
            Serial.println("Failed to connect, deleted client");
            return false;
        }
    }

    if(!pClient->isConnected()) {
        if (!pClient->connect(advDevice)) {
            Serial.println("Failed to connect");
            return false;
        }
    }

    Serial.print("Connected to: ");
    Serial.println(pClient->getPeerAddress().toString().c_str());
    Serial.print("RSSI: ");
    Serial.println(pClient->getRssi());

    /** Now we can read/write/subscribe the charateristics of the services we are interested in */
    NimBLERemoteService* pSvc = nullptr;
    NimBLERemoteCharacteristic* pChr = nullptr;
    NimBLERemoteDescriptor* pDsc = nullptr;

    pSvc = pClient->getService(SWC_SRVI_UUID);
    if(pSvc){
        NimBLERemoteCharacteristic * VWC_chr = pSvc->getCharacteristic(SWC_VWC_CHAR_UUID);
        String SWC_read_string = (String) VWC_chr->readValue().c_str();
        SWC_read = format_SWC( (unsigned char*) SWC_read_string.c_str());
        //SWC_read = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>("1.23"));

        time_last_read = millis();
        //time_last_read = 69L;
        unsigned long timestamp = time_last_read-time_start;
        //timestamp = time_last_read;
        printf("Will try to save the SWC: %c%c%c%c%c and the timestamp %lu \n",SWC_read[0],SWC_read[1],SWC_read[2],SWC_read[3],SWC_read[4],timestamp);

    }


    return true;
}

void setup (){
    Serial.begin(115200);
    Serial.println("Starting NimBLE Client");
    fileMane.mount();

    //Interrupts and pins
    pinMode(PRINTING_ON_TRIGGER_PIN,INPUT);
    attachInterrupt(digitalPinToInterrupt(PRINTING_ON_TRIGGER_PIN),printon_interrupt,HIGH);

    pinMode(XPERIMENT_ON_TRIGGER_PIN,INPUT);
    attachInterrupt(digitalPinToInterrupt(XPERIMENT_ON_TRIGGER_PIN),xperimenton_interrupt,HIGH);

    pinMode(RESTART_TRIGGER_PIN,INPUT);
    attachInterrupt(digitalPinToInterrupt(RESTART_TRIGGER_PIN),restart_interrupt,HIGH);


    /** Initialize NimBLE, no device name spcified as we are not advertising */
    NimBLEDevice::init(BLE_CLIENT_NAME);

    /** Set the IO capabilities of the device, each option will trigger a different pairing method.
     *  BLE_HS_IO_KEYBOARD_ONLY    - Passkey pairing
     *  BLE_HS_IO_DISPLAY_YESNO   - Numeric comparison pairing
     *  BLE_HS_IO_NO_INPUT_OUTPUT - DEFAULT setting - just works pairing
     */
    //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY); // use passkey
    //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO); //use numeric comparison

    /** 2 different ways to set security - both calls achieve the same result.
     *  no bonding, no man in the middle protection, secure connections.
     *
     *  These are the default values, only shown here for demonstration.
     */
    //NimBLEDevice::setSecurityAuth(false, false, true);
    NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);

    /** Optional: set the transmit power, default is 3db */
#ifdef ESP_PLATFORM
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
#else
    NimBLEDevice::setPower(9); /** +9db */
#endif

    /** Optional: set any devices you don't want to get advertisments from */
    // NimBLEDevice::addIgnored(NimBLEAddress ("aa:bb:cc:dd:ee:ff"));

    /** create new scan */
    NimBLEScan* pScan = NimBLEDevice::getScan();

    /** create a callback that gets called when advertisers are found */
    pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());

    /** Set scan interval (how often) and window (how long) in milliseconds */
    pScan->setInterval(0);
    pScan->setWindow(15);

    /** Active scan will gather scan response data from advertisers
     *  but will use more energy from both devices
     */
    pScan->setActiveScan(true);
}


void loop (){

    if(restart_on)
    {
        log_e("Restarting");
        bool res1,res2,finish = false;
        do{
        delay(500);
        res1 = overwrite_value_array(SWC_VALUE_ARRAY_SIZE,SWC_VALUE_ARRAY_PATH);
        delay(1500);
        res2 = overwrite_value_array(TIMESTEP_VALUE_ARRAY_SIZE,TIMESTEP_VALUE_ARRAY_PATH);
        finish = res1 && res2;
        }
        while(!finish);


        esp_system_abort("Restarting experiment");

    }

    while(print_on)
    {
        if(NimBLEDevice::getScan()->isScanning())
        {
            were_scanning_print = true;
             NimBLEDevice::getScan()->stop();}

        unsigned char * print_TIMESTEP = (unsigned char* )malloc(sizeof(char) * TIMESTEP_VALUE_ARRAY_SIZE);
        fileMane.load_file(TIMESTEP_VALUE_ARRAY_PATH,reinterpret_cast<unsigned char *>(print_TIMESTEP),TIMESTEP_VALUE_ARRAY_SIZE-1);
        for(int i = 0; i<120/4;i += 4)
        {
            unsigned long long_rep = (print_TIMESTEP[i]) | (print_TIMESTEP[i+1]<< 8) | (print_TIMESTEP[i+2] << 16) | (print_TIMESTEP[i+3] << 24);
            printf("%lu        ",long_rep);
        }

        vTaskDelay(1000/portTICK_PERIOD_MS);

        printf("\n");

        free(print_TIMESTEP);

        char * print_SWC = (char* )malloc(sizeof(char) * SWC_VALUE_ARRAY_SIZE);
        fileMane.load_file(SWC_VALUE_ARRAY_PATH,reinterpret_cast<unsigned char *> (print_SWC),SWC_VALUE_ARRAY_SIZE-1);
        for(int i = 0; i<100;i++)
        {
            printf("%c",print_SWC[i]);
        }

        free(print_SWC);
        printf("\n");
        vTaskDelay(TIME_DELAY_AFTER_PRINTING_mS/portTICK_PERIOD_MS);
        if(!print_on && were_scanning_print)
        {
            NimBLEDevice::getScan()->start(scanTime,scanEndedCB);
            log_e("Done printing\n");
        }
    }



    if(xperiment_on)
    {
        switch(state)
        {
            case STATE_IDLE:

                log_e("%i",xperiment_on);
                log_e("State: Idle");

                if(!print_on)
                {
                    //TODO: Turn on LED
                    NimBLEDevice::getScan()->start(scanTime,scanEndedCB);
                    state = STATE_LISTENING;
                }

                break;

            case STATE_LISTENING:


                //If we can connect to our server
                if(doConnect)
                {
                    log_e("State: Listening\n");
                    connectToServer();
                    if(xperiment_on){
                    insert_at_carriage_return_and_save(TIMESTEP_VALUE_ARRAY_PATH,(time_last_read-time_start),SIZE_OF_TIMESTAMP_AFTER_FORMATTING,TIMESTEP_VALUE_ARRAY_SIZE,timestep_count*SIZE_OF_TIMESTAMP_AFTER_FORMATTING,&timestep_count);
                    insert_at_carriage_return_and_save(SWC_VALUE_ARRAY_PATH,SWC_read,SIZE_OF_SWC_AFTER_FORMATTING,SWC_VALUE_ARRAY_SIZE,SWC_counter*SIZE_OF_SWC_AFTER_FORMATTING,&SWC_counter);
                    doConnect = false;
                    state = STATE_VALUE_RECEIVED;}
                }

                break;

            case STATE_VALUE_RECEIVED:

                printf("State: Received\n");

                //TODO: Check if array is full

                if(xperiment_on)
                {
                    state = STATE_LISTENING;
                }
                else
                {
                    state = STATE_IDLE;
                }

                break;
        }

    }
    else{
        log_e("Experiment off");
        doConnect = false;
        if(NimBLEDevice::getScan()->isScanning()){NimBLEDevice::getScan()->stop();}
    }



}
