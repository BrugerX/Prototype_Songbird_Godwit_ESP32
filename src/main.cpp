#include "Arduino.h"
#include "TBeamPower.h"
#include "NonGeneric_Sensor.h"
#include <string>
#include "SENSOR_SETTINGS.h"

#define BATTERY_PWR_VLTG_CHAR_UUID "0002"
#define BATTERY_PWR_WARN_CHAR_UUID "0003"
#define BATTERY_PWR_SRVI_UUID "0001"

#define SWC_SRVI_UUID "0004"
#define SWC_CALIBRTNA_CHAR_UUID "0005"
#define SWC_CALIBRTNB_CHAR_UUID "0006"
#define SWC_READ_CHAR_UUID "0007"
#define SWC_READ_FREQ_CHAR_UUID "0008"

#define TEMP_SRVI_UUID "0010"
#define TEMP_READ_CHAR_UUID "0011"
#define TEMP_READ_FREQ_CHAR_UUID "0012"



#define BATTERY_THRESHOLD 2.7

auto * batman = new TBeamPower();
auto * SWC_sensor = new Simple_SWC_Sensor(SWC_SENSR_READ_PIN,SWC_SENSR_PWR_PIN);

/** NimBLE_Server Demo:
 *
 *  Demonstrates many of the available features of the NimBLE server library.
 *
 *  Created: on March 22 2020
 *      Author: H2zero
 *
*/

#include "NimBLEDevice.h"

static NimBLEServer* pServer;

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
        Serial.println("Client connected");
        Serial.println("Multi-connect support: start advertising");
        NimBLEDevice::startAdvertising();
    };
    /** Alternative onConnect() method to extract details of the connection.
     *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
     */
    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
        Serial.print("Client address: ");
        Serial.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
        /** We can use the connection handle here to ask for different connection parameters.
         *  Args: connection handle, min connection interval, max connection interval
         *  latency, supervision timeout.
         *  Units; Min/Max Intervals: 1.25 millisecond increments.
         *  Latency: number of intervals allowed to skip.
         *  Timeout: 10 millisecond increments, try for 5x interval time for best results.
         */
        pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);
    };
    void onDisconnect(NimBLEServer* pServer) {
        Serial.println("Client disconnected - start advertising");
        NimBLEDevice::startAdvertising();
    };
    void onMTUChange(uint16_t MTU, ble_gap_conn_desc* desc) {
        Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
    };

/********************* Security handled here **********************
****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest(){
        Serial.println("Server Passkey Request");
        /** This should return a random 6 digit number for security
         *  or make your own static passkey as done here.
         */
        return 123456;
    };

    bool onConfirmPIN(uint32_t pass_key){
        Serial.print("The passkey YES/NO number: ");Serial.println(pass_key);
        /** Return false if passkeys don't match. */
        return true;
    };

    void onAuthenticationComplete(ble_gap_conn_desc* desc){
        /** Check that encryption was successful, if not we disconnect the client */
        if(!desc->sec_state.encrypted) {
            NimBLEDevice::getServer()->disconnect(desc->conn_handle);
            Serial.println("Encrypt connection failed - disconnecting client");
            return;
        }
        Serial.println("Starting BLE work!");
    };
};

/** Handler class for characteristic actions */
class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {


    void onRead(NimBLECharacteristic* pCharacteristic){
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    };

    void onWrite(NimBLECharacteristic* pCharacteristic) {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onWrite(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());

    };
    /** Called before notification or indication is sent,
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic* pCharacteristic) {
        Serial.println("Sending notification to clients");
    };


    /** The status returned in status is defined in NimBLECharacteristic.h.
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code) {
        String str = ("Notification/Indication status code: ");
        str += status;
        str += ", return code: ";
        str += code;
        str += ", ";
        str += NimBLEUtils::returnCodeToString(code);
        Serial.println(str);
    };

    void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue) {
        String str = "Client ID: ";
        str += desc->conn_handle;
        str += " Address: ";
        str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
        if(subValue == 0) {
            str += " Unsubscribed to ";
        }else if(subValue == 1) {
            str += " Subscribed to notfications for ";
        } else if(subValue == 2) {
            str += " Subscribed to indications for ";
        } else if(subValue == 3) {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID()).c_str();

        Serial.println(str);
    };
};

/** Handler class for descriptor actions */
class DescriptorCallbacks : public NimBLEDescriptorCallbacks {
    void onWrite(NimBLEDescriptor* pDescriptor) {
        std::string dscVal = pDescriptor->getValue();
        Serial.print("Descriptor witten value:");
        Serial.println(dscVal.c_str());
    };

    void onRead(NimBLEDescriptor* pDescriptor) {
        Serial.print(pDescriptor->getUUID().toString().c_str());
        Serial.println(" Descriptor read");
    };
};


/** Define callback instances globally to use for multiple Charateristics \ Descriptors */
static DescriptorCallbacks dscCallbacks;
static CharacteristicCallbacks chrCallbacks;


void setup() {
    Serial.begin(115200);
    Serial.println("Starting NimBLE Server");

    batman->begin();

    /** sets device name */
    NimBLEDevice::init("Din Mor Er Grim");

    /** Optional: set the transmit power, default is 3db */
#ifdef ESP_PLATFORM
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
#else
    NimBLEDevice::setPower(9); /** +9db */
#endif

    /** Set the IO capabilities of the device, each option will trigger a different pairing method.
     *  BLE_HS_IO_DISPLAY_ONLY    - Passkey pairing
     *  BLE_HS_IO_DISPLAY_YESNO   - Numeric comparison pairing
     *  BLE_HS_IO_NO_INPUT_OUTPUT - DEFAULT setting - just works pairing
     */
    //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY); // use passkey
    //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO); //use numeric comparison

    /** 2 different ways to set security - both calls achieve the same result.
     *  no bonding, no man in the middle protection, secure connections.
     *
     *  These are the default values, only shown here for demonstration.
     */
    //NimBLEDevice::setSecurityAuth(false, false, true);
    NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());



    NimBLEService* pDeadService = pServer->createService("DEAD");
    NimBLECharacteristic* pBeefCharacteristic = pDeadService->createCharacteristic(
            "BEEF",
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE |
            /** Require a secure connection for read and write access */
            NIMBLE_PROPERTY::READ_ENC |  // only allow reading if paired / encrypted
            NIMBLE_PROPERTY::WRITE_ENC   // only allow writing if paired / encrypted
    );


    pBeefCharacteristic->setValue("Burger");
    pBeefCharacteristic->setCallbacks(&chrCallbacks);

    /** 2904 descriptors are a special case, when createDescriptor is called with
     *  0x2904 a NimBLE2904 class is created with the correct properties and sizes.
     *  However we must cast the returned reference to the correct type as the method
     *  only returns a pointer to the base NimBLEDescriptor class.
     */
    NimBLE2904* pBeef2904 = (NimBLE2904*)pBeefCharacteristic->createDescriptor("2904");
    pBeef2904->setFormat(NimBLE2904::FORMAT_UTF8);
    pBeef2904->setCallbacks(&dscCallbacks);
    /**
     * Temperature services
     * Includes:
     * Temp_read: Readable, Notificationable: The temperature given in Celsius
     * Temp_read_freq: Writeable, readable: The frequence between reads
     */

    NimBLEService * pTempService = pServer->createService(TEMP_SRVI_UUID);
    NimBLECharacteristic * pTempReadChar = pTempService->createCharacteristic(
            TEMP_READ_CHAR_UUID,
            NIMBLE_PROPERTY::READ|NIMBLE_PROPERTY::NOTIFY
            );

    pTempReadChar->setValue(0);
    pTempReadChar->setCallbacks(&chrCallbacks);

    NimBLECharacteristic * pTempFreqChar = pTempService->createCharacteristic(
            TEMP_READ_FREQ_CHAR_UUID,
            NIMBLE_PROPERTY::READ|NIMBLE_PROPERTY::WRITE
            );
    pTempFreqChar->setValue(360);
    pTempFreqChar->setCallbacks(&chrCallbacks);
    /**
     * SWC services.
     * Includes:
     * SWC_read char: Readable, indicationable: Is the soil water content reading
     * SWC_read_freq: Readable,writeable: The frequency at which we indicate our SWC reading
     * SWC_A: Writeable,readable: The A in the linear regression
     * SWC_B: Writeable,readable: The B in the linear regression
     */

    NimBLEService * pSWCService = pServer->createService(SWC_SRVI_UUID);
    NimBLECharacteristic * pCalibrtnAChr = pSWCService->createCharacteristic(
            SWC_CALIBRTNA_CHAR_UUID,
            NIMBLE_PROPERTY::READ| NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ_ENC|NIMBLE_PROPERTY::WRITE_ENC
    );

    pCalibrtnAChr->setValue("CalbritnA");
    pCalibrtnAChr->setCallbacks(&chrCallbacks);

    NimBLECharacteristic * pCalibrtnBChr = pSWCService->createCharacteristic (
            SWC_CALIBRTNB_CHAR_UUID,
            NIMBLE_PROPERTY::READ| NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ_ENC|NIMBLE_PROPERTY::WRITE_ENC
    );


    pCalibrtnBChr->setValue("CalibrtnB");
    pCalibrtnBChr->setCallbacks(&chrCallbacks);

    NimBLECharacteristic * pSWCReadChr = pSWCService ->createCharacteristic(
            SWC_READ_CHAR_UUID,
            NIMBLE_PROPERTY::READ|NIMBLE_PROPERTY::INDICATE
            );

    pSWCReadChr->setValue("SWC_READ");
    pSWCReadChr->setCallbacks(&chrCallbacks);

    NimBLECharacteristic * pSWCFreqChr = pSWCService->createCharacteristic
            (
                    SWC_READ_FREQ_CHAR_UUID,
                    NIMBLE_PROPERTY::READ|NIMBLE_PROPERTY::WRITE
                    );
    pSWCFreqChr->setValue(100);
    pSWCFreqChr->setCallbacks(&chrCallbacks);

    /**
     * BATTERY SERVICES
     */
    NimBLEService* pBatriService = pServer->createService(BATTERY_PWR_SRVI_UUID);
    NimBLECharacteristic* pBatriVltgCharacteristic = pBatriService->createCharacteristic(
            BATTERY_PWR_VLTG_CHAR_UUID,
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY
    );

    pBatriVltgCharacteristic->setValue("0");
    pBatriVltgCharacteristic->setCallbacks(&chrCallbacks);

    NimBLECharacteristic * pBatriWarnCharacteristic = pBatriService->createCharacteristic(
            BATTERY_PWR_WARN_CHAR_UUID,
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY
    );

    pBatriWarnCharacteristic->setValue("0");
    pBatriWarnCharacteristic->setCallbacks(&chrCallbacks);


    /** Note a 0x2902 descriptor MUST NOT be created as NimBLE will create one automatically
     *  if notification or indication properties are assigned to a characteristic.
     */

    /** Custom descriptor: Arguments are UUID, Properties, max length in bytes of the value */
    NimBLEDescriptor* pC01Ddsc = pBatriVltgCharacteristic->createDescriptor(
            "C01D",
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE|
            NIMBLE_PROPERTY::WRITE_ENC, // only allow writing if paired / encrypted
            40
    );
    pC01Ddsc->setValue("Send it back!");
    pC01Ddsc->setCallbacks(&dscCallbacks);

    /** Start the services when finished creating all Characteristics and Descriptors */
    pDeadService->start();
    pBatriService->start();
    pSWCService->start();
    pTempService->start();

    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    /** Add the services to the advertisment data **/
    pAdvertising->addServiceUUID(pDeadService->getUUID());
    pAdvertising->addServiceUUID(pBatriService->getUUID());
    pAdvertising->addServiceUUID(pSWCService->getUUID());
    pAdvertising->addServiceUUID(pTempService->getUUID());
    /** If your device is battery powered you may consider setting scan response
     *  to false as it will extend battery life at the expense of less data sent.
     */
    pAdvertising->setScanResponse(true);
    pAdvertising->start();

    Serial.println("Advertising Started");
}


void loop() {
    if(pServer->getConnectedCount()) {

        //Everything related to power management
        NimBLEService* pBatriSvc = pServer->getServiceByUUID(BATTERY_PWR_SRVI_UUID);
        if(pBatriSvc) {
            NimBLECharacteristic* pChr = pBatriSvc->getCharacteristic(BATTERY_PWR_VLTG_CHAR_UUID);

            log_i("%s","We got the Batri service");

            int btriVoltage = batman->get_battery_voltage();

            if(pChr) {

                //We don't want to communicate a negative value (?)
                if(btriVoltage == BATTERY_NOT_CONNECTED_CODE)
                {
                    pChr->setValue("0");
                }
                else
                {
                    pChr->setValue(std::to_string(batman->get_battery_voltage()));
                }

                log_i("%s",CCIDDesc->getStringValue());
                if(pChr->getSubscribedCount()>0)
                {
                    pChr->notify(true);
                }
            }


            NimBLECharacteristic * pBtriWarnChar = pBatriSvc->getCharacteristic(BATTERY_PWR_WARN_CHAR_UUID);

            //We now check to see if we should warn the client of the low battery power
            if(pBtriWarnChar)
            {
                if(btriVoltage<BATTERY_THRESHOLD && btriVoltage != BATTERY_NOT_CONNECTED_CODE)
                {
                    pBtriWarnChar->setValue("1");
                    pBtriWarnChar->notify(true);
                }
                else if (pBtriWarnChar->getValue() == "1")
                {
                    pBtriWarnChar->setValue("0");
                    pBtriWarnChar->notify(true);
                }
            }

        }

        NimBLEService * pSWCSvc = pServer->getServiceByUUID(SWC_SRVI_UUID);
        if(pSWCSvc)
        {
            NimBLECharacteristic* pSWCReadChr = pSWCSvc->getCharacteristic(SWC_READ_CHAR_UUID);
            if(pSWCReadChr->getSubscribedCount()>0)
            {
                pSWCReadChr->notify(0);
            }
        }

    }


    delay(2000);
}