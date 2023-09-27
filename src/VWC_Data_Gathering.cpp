/*
#include "Arduino.h"
#include "NonGeneric_Sensor.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#define wtr_sensr_pwr_pin 34
#define btn_activt_read_pin 7
#define xperiment_done_LED 2

#define swc_nr_samples 1600
#define temp_nr_samples 3

// GPIO where the DS18B20 is connected to
const int oneWireBus = 12;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

//Setup our SWC sensor
auto * SWC_sensor = new SPI_SWC_Sensor(wtr_sensr_pwr_pin);

int swc_read_value;
float swc_average;

float temperature_read_value_c;
float temperature_average_c;

float calculate_sample_var(float mean,int sample_size, float * samples)
{
    float var = 0;

    for(int i = 0; i<sample_size;i++)
    {
        float sample = samples[i];
        var += (mean-sample)*(mean-sample);
    }

    return var/(sample_size - 1);
}

float calculate_sample_var(float mean,int sample_size, int * samples)
{
    float var = 0;
    float sum = 0;

    for(int i = 0; i<sample_size;i++)
    {
        float sample = (float) samples[i];
        sum += (mean-sample)*(mean-sample);
    }

    return sum/(sample_size - 1);
}


void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("PROGRAM BEGIN");
    pinMode(wtr_sensr_pwr_pin, OUTPUT);
    pinMode(xperiment_done_LED,OUTPUT);

    pinMode(btn_activt_read_pin,INPUT);


    sensors.begin();

}

void loop() {
    int xperiment_done = 0;
    digitalWrite(xperiment_done_LED,HIGH);

    if(!digitalRead(btn_activt_read_pin)){


        //Otherwise the button reacts too fast
        delay(1000);

        //Now we wait for the second button press
        while(xperiment_done == 0){
            digitalWrite(wtr_sensr_pwr_pin,HIGH);
            swc_read_value = SWC_sensor->read_sensor();


            sensors.requestTemperatures();

            temperature_read_value_c = sensors.getTempCByIndex(0);

            Serial.println(swc_read_value);
            Serial.println(temperature_read_value_c);

            if(!digitalRead(btn_activt_read_pin))
            {
                long swc_sum = 0;
                float temperature_sum_c = 0;

                float temperature_reads_arr[temp_nr_samples];
                int swc_reads_arr[swc_nr_samples];

                int temperature_error_measurement;

                digitalWrite(xperiment_done_LED,LOW);
                Serial.println("Taking samples!");

                //determine the highest sample size
                int highest_nr_samples;
                if(temp_nr_samples>swc_nr_samples)
                {
                    highest_nr_samples = temp_nr_samples;
                } else
                {
                    highest_nr_samples = swc_nr_samples;
                }

                //Now we start taking samples
                for(int i = 0; i<highest_nr_samples;i++)
                {
                    if(i<swc_nr_samples)
                    {

                        swc_read_value = SWC_sensor->read_sensor();
                        swc_sum += swc_read_value;
                        swc_reads_arr[i] = swc_read_value;

                    }


                    if(i<temp_nr_samples)
                    {

                        sensors.requestTemperatures();
                        temperature_read_value_c = sensors.getTempCByIndex(0);
                        temperature_sum_c += temperature_read_value_c;
                        temperature_reads_arr[i] = temperature_read_value_c;
                        delay(100);


                        if(temperature_read_value_c<0)
                        {
                            i = highest_nr_samples;
                            temperature_error_measurement = 1;
                        }
                    }


                    delay(10);
                }

                swc_average = swc_sum/swc_nr_samples;
                temperature_average_c = temperature_sum_c/temp_nr_samples;
                Serial.println("SWC AVERAGE:");
                Serial.println(swc_average);
                Serial.println("SWC VAR");
                Serial.println(calculate_sample_var(swc_average,swc_nr_samples,swc_reads_arr));

                if(!temperature_error_measurement)
                {
                    Serial.println("TEMPERATURE AVERAGE CELSIUS:");
                    Serial.println(temperature_average_c);
                    Serial.println("Temperature VAR");
                    Serial.println(calculate_sample_var(temperature_average_c,temp_nr_samples,temperature_reads_arr));
                }
                else
                {
                    Serial.println("ERROR IN TEMPERATURE MEASUREMENT");
                }
                xperiment_done = 1;
                temperature_error_measurement = 0;
                delay(1000);

                digitalWrite(wtr_sensr_pwr_pin,LOW);
                Serial.print("Ready again");
            }
            delay(100);
        }
    }
}
*/