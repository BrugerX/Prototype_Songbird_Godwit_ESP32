//
// Created by DripTooHard on 20-05-2023.
//


#include <Arduino.h>
#include <unity.h>
#include <SPI.h>
#include "NonGeneric_Sensor.h"

auto * SPI_class = new SPIClass(VSPI);

auto * SPI_sensor = new SPI_SWC_Sensor(33);



void setUp(void) {
    // set stuff up here
}


void tearDown(void) {
    // delete stuff down here
}

void readSensor(void)
{

    SPI_sensor->turn_on_sensor();
    delay(10000);
    log_e("Sensor turned on");
    delay(10000);

    for(int i = 0; i<10;i++)
    {
        float result = SPI_sensor->read_sensor_v();
        log_e("Result: %f",result);
        delay(1000);
    }

}

void takeSampleAverage(void)
{
    log_e("Result of average: %f",SPI_sensor->take_sample_avg(1600,10));
}

void setup()
{
    delay(5000); // service delay


    //DON'T PUT ANYTHING BEFORE THIS EXCEPT FOR DELAY!!!!
    UNITY_BEGIN(); //Define stuff after this
    /*
    pinMode(custom_SS,OUTPUT);
    SPI_class->begin(custom_CLK,custom_MISO,custom_MOSI,custom_SS);


    digitalWrite(custom_SS,HIGH);
    SPI_class->setBitOrder(MSBFIRST); //MSB goes first out
    SPI_class->setClockDivider(SPI_CLOCK_DIV16); //1Mhz SPI clock
    */
    RUN_TEST(readSensor);
    RUN_TEST(takeSampleAverage);
    UNITY_END(); // stop unit testing
}

void loop()
{
}