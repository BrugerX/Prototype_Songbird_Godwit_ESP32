//
// Created by DripTooHard on 08-07-2023.
//

#ifndef PROTOBLETEST_NONGENERIC_SENSOR_H
#define PROTOBLETEST_NONGENERIC_SENSOR_H

#include "Arduino.h"
#include "arrayUtil.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define ADC_RESOLUTION 4096
#define ADC_REF_VOLT  3.3


/**
 * A simple analog sensor, that has a pin it reads from.
 */
class Simple_Analog_Sensor{

protected:
    int read_pin;

    virtual bool isADCPin(int GPIO_pin);


public:


    ~Simple_Analog_Sensor(){};

    //Returns the voltage reading
    virtual float read_voltage();

    //Returns the raw reading
    virtual int read();


    virtual int set_read_pin(int new_pin);

    virtual int get_read_pin();


    Simple_Analog_Sensor(int read_pin){};




};

/**
 *
 */
class Simple_SWC_Sensor : public Simple_Analog_Sensor
{

protected:
    int pwr_pin{};

    void turn_on_sensor();

    void turn_off_sensor();

    bool isADCPin(int GPIO_pin);

    /**
     * mode [IN]: 0 for turn off, 1 for turn on
     */
    void turn_sensor(int mode);


public:

    /**
     * @pre: readPin is an ADC pin
     * @param readPin
     * @param pwr_pin
    */
    Simple_SWC_Sensor(int read_Pin, int pwr_pin) : Simple_Analog_Sensor(read_Pin) {
        adc_power_acquire();

        if(!isADCPin(read_Pin)){
            log_e("%s","ERROR IN INITIALIZING SIMPLE_SWC_SENSOR: PIN IS NOT AN ADC");
            //throw std::domain_error("PIN IS NOT AN ADC");
        }
        //pinMode(read_Pin,INPUT);

        this->read_pin = read_Pin;

        pinMode(pwr_pin,OUTPUT);

        this->pwr_pin = pwr_pin;
    };


    /**
     * @post: Sensor is turned off
     * @attention: There is a 1 second delay before it reads the sensor value
     * @return
     */
    int read();

    float read_voltage();

    int set_read_pin(int new_pin);

    int get_read_pin();

    //Method that indicates whether the sensor should be changed.
    int isTimeToChange();

    int get_pwr_pin();

    int set_pwr_pin(int new_pin);





};


#endif //PROTOBLETEST_NONGENERIC_SENSOR_H
