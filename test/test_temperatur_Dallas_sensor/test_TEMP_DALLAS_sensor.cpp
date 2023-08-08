//
// Created by DripTooHard on 25-07-2023.
//
//
// Created by DripTooHard on 20-05-2023.
//


#include <Arduino.h>
#include <unity.h>
#include <SENSOR_MACROS.h>
#include <OneWire.h>
#include <DallasTemperature.h>


OneWire * uneWire = new OneWire(DALLAS_TEMP_BUS_PIN);
DallasTemperature * TEMPSens = new DallasTemperature(uneWire);


void setUp(void) {
    // set stuff up here
}


void tearDown(void) {
    // delete stuff down here
    TEMPSens->requestTemperatures();
}

void readSensor(void)
{
    while(true)
    {
        TEMPSens->requestTemperatures();
        delay(1000);
        log_e("Result of the reading: %f", TEMPSens->getTempCByIndex(0));
    }
}

void setup()
{
    delay(5000); // service delay


    //DON'T PUT ANYTHING BEFORE THIS EXCEPT FOR DELAY!!!!
    UNITY_BEGIN(); //Define stuff after this
    TEMPSens->begin();
    RUN_TEST(readSensor);
    UNITY_END(); // stop unit testing
}

void loop()
{
}