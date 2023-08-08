//
// Created by DripTooHard on 08-07-2023.
//

#ifndef PROTOBLETEST_NONGENERIC_SENSOR_H
#define PROTOBLETEST_NONGENERIC_SENSOR_H

#include "Arduino.h"
#include "arrayUtil.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "DallasTemperature.h"
#include <OneWire.h>
#include <SPI.h>
#include "SENSOR_MACROS.h"

class Simple_Sensor
        {


protected:


    virtual uint16_t * take_samples(int sample_size,int delay_wait);

public:


    ~Simple_Sensor(){};

    //Returns the raw reading
    virtual uint16_t read_sensor();

    virtual float take_sample_avg(int sample_size,int delay_wait);


    Simple_Sensor(){};



};

/**
 * A simple analog sensor
 */
class Simple_Analog_Sensor : public Simple_Sensor
        {

protected:
    int ADC_resolution = ADC_RESOLUTION;
    float ref_voltage = ADC_REF_VOLT;

    float from_ADC_to_voltage(uint16_t ADC_value);


public:


    ~Simple_Analog_Sensor(){};

    //Returns the voltage reading
    virtual float read_sensor_v();



    Simple_Analog_Sensor(){};




};



class SPI_SWC_Sensor : public Simple_Analog_Sensor
{
protected:

    uint16_t * take_samples(int sample_size, int delay_wait);
    private:

    SPIClass * SPI_class = new SPIClass(VSPI);

    bool isSSHigh = false;
    bool isSWCHigh = false;


    int pwr_pin;

    void turn_on_SS();

    void turn_off_SS();


    uint8_t transfer(uint8_t outgoing_data);


    public:


    void turn_on_sensor();

    void turn_off_sensor();


    uint16_t read_sensor();

    float read_sensor_v();

    float take_sample_avg(int sample_size,int delay_wait);


    SPI_SWC_Sensor(int init_pwr_pin)
    {
        pwr_pin = init_pwr_pin;
        pinMode(custom_SS,OUTPUT);
        pinMode(pwr_pin,OUTPUT);

        SPI_class->begin(custom_CLK,custom_MISO,custom_MOSI,custom_SS);


        digitalWrite(custom_SS,HIGH);
        SPI_class->setBitOrder(MSBFIRST); //MSB goes first out
        SPI_class->setClockDivider(SPI_CLOCK_DIV16); //1Mhz SPI clock
    };




};


#endif //PROTOBLETEST_NONGENERIC_SENSOR_H
