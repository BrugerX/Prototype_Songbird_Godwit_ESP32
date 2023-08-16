//
// Created by DripTooHard on 08-07-2023.
//

#include "NonGeneric_Sensor.h"

float Simple_Analog_Sensor::from_ADC_to_voltage(uint16_t ADC_value)
{
    return (float) ADC_value/(ADC_resolution/ref_voltage);
}

uint16_t * SPI_SWC_Sensor::take_samples(int sample_size, int delay_wait)
{
    uint16_t sample[sample_size];
    for(int i = 0; i<sample_size;i++)
    {
        sample[i] = read_sensor_v();
    }

    return sample;
}

float SPI_SWC_Sensor::take_sample_avg(int sample_size, int delay_wait)
{
    float sum;

    for(int i = 0; i<sample_size;i++)
    {
        sum += read_sensor_v();
        delay(delay_wait);
    }

    return sum/sample_size;
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

    byte byte0,byte1;
    uint16_t word;

    turn_on_SS();
    transfer(0x18); //Configuration bits
    byte0 = transfer(0x00); //Now when we send something, we will get the ADC read back
    byte1 = transfer(0x00);

    turn_off_SS();

    word = ( byte0 << 8 | byte1 ) & 0b111111111111;
    return word;
}

float  SPI_SWC_Sensor::read_sensor_v()
{
    float sensor_value = from_ADC_to_voltage(read_sensor());
    return sensor_value;
}
