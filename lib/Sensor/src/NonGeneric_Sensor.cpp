//
// Created by DripTooHard on 08-07-2023.
//

#include "NonGeneric_Sensor.h"




int Simple_SWC_Sensor::get_read_pin()
{
    return this->read_pin;
}

int Simple_SWC_Sensor::set_read_pin(int new_pin)
{
    this->read_pin = new_pin;
    return 1;
}

int Simple_SWC_Sensor::set_pwr_pin(int new_pin)
{
    this->pwr_pin = new_pin;
    return 1;
}

int Simple_SWC_Sensor::get_pwr_pin()
{
    return this->pwr_pin;
}

void Simple_SWC_Sensor::turn_off_sensor()
{
    digitalWrite(pwr_pin,LOW);
}

void Simple_SWC_Sensor::turn_on_sensor()
{
    digitalWrite(pwr_pin,HIGH);
}

void Simple_SWC_Sensor::turn_sensor(int mode)
{
    if(mode == 1)
    {
        turn_on_sensor();
    }
    else
    {
        turn_off_sensor();
    }
}

bool Simple_SWC_Sensor::isADCPin(int GPIO_pin)
{

    int esp32_analogPins[] = {0,2,4,12,13,14,15,25,26,27,32,33,34,35,36,37,38,39};
    return contains(GPIO_pin,esp32_analogPins,18);
}

int Simple_SWC_Sensor::read()
{
    int res;
    turn_sensor(1);

    static esp_adc_cal_characteristics_t adc1_chars;
    uint32_t voltage;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_10, 0, &adc1_chars);
    adc1_config_width(ADC_WIDTH_BIT_10);   //Range 0-1023
    adc1_config_channel_atten(ADC1_CHANNEL_5,ADC_ATTEN_DB_11);  //ADC_ATTEN_DB_11 = 0-3,6V
    return adc1_get_raw( (adc1_channel_t ) ADC1_CHANNEL_5 ); //Read analog

    //It takes some time for the sensor to properly read
    //TODO: Create something that only starts reading once the running variance reaches some maximum
    //delay(2000);
    res = analogRead(read_pin);
    turn_sensor(0);

    return res;
}

float Simple_SWC_Sensor::read_voltage()
{
    return ADC_REF_VOLT/ADC_RESOLUTION * read();
}

int Simple_SWC_Sensor::isTimeToChange()
{
    log_e("%s","NUFFIN");
}


void SPI_SWC_Sensor::turn_on_SS()
{
    digitalWrite(custom_SS,LOW);
    isSSHigh = true;
}

void SPI_SWC_Sensor::turn_off_SS()
{
    digitalWrite(custom_SS,HIGH);
    isSSHigh = false;
}

void SPI_SWC_Sensor::turn_on_sensor()
{
    digitalWrite(pwr_pin,HIGH);
    isSWCHigh = true;
}

void SPI_SWC_Sensor::turn_off_sensor()
{
    isSWCHigh = true;
}

/**
 * @pre The SS pin is high
 * @param outgoing_data
 * @return
 */

uint8_t SPI_SWC_Sensor::transfer(uint8_t outgoing_data)
{
    if(isSSHigh)
    {
        byte inbound_data;
        inbound_data = SPI_class->transfer(outgoing_data);
        return inbound_data;
    }

    throw std::logic_error("SLAVE SELECT WAS NOT HIGH WHEN TRYING TO TRANSFER DATA");
}

uint16_t  SPI_SWC_Sensor::read_sensor()
{
    turn_on_sensor();

    byte byte0,byte1;
    uint16_t word;

    turn_on_SS();
    transfer(0x18); //Configuration bits
    byte0 = transfer(0x00); //Now when we send something, we will get the ADC read back
    byte1 = transfer(0x00);

    turn_off_SS();
    turn_off_sensor();

    word = ( byte0 << 8 | byte1 ) & 0b111111111111;
    return word;
}

float  SPI_SWC_Sensor::read_sensor_v()
{
    float sensor_value = read_sensor()/(ADC_resolution/ref_voltage);
    return sensor_value;
}
