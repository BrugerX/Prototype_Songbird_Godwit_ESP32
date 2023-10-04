//
// Created by DripTooHard on 03-10-2023.
//

#include <DataLogging.h>

//
// Created by DripTooHard on 20-05-2023.
//


#include <Arduino.h>
#include <unity.h>
#include <DataLogging.h>
#include <UnsignedStringUtility.h>

#define size_of_premade_string 8

char * premade_string_to_write = "12.34";
SPIFFSFileManager& fileMan = SPIFFSFileManager::get_instance();

void setUp(void) {
    // set stuff up here
}


void tearDown(void) {
    // delete stuff down here
}

void formating_stop_char(void)
{
    int size_of_SWC_value_input = 5;
    char * SWC_value_input = "12.34";

    unsigned char * SWC_value_output = format_value_for_FS(reinterpret_cast<unsigned char *>(SWC_value_input),size_of_SWC_value_input);

    for(int i = 0; i<size_of_SWC_value_input;i++)
    {
        TEST_ASSERT_EQUAL((unsigned char) SWC_value_input[i], SWC_value_output[i]);
    }

    TEST_ASSERT_TRUE(SWC_value_output[size_of_SWC_value_input] == END_OF_VALUE_CHAR);
    TEST_ASSERT_TRUE(SWC_value_output[size_of_SWC_value_input+1] == END_OF_STRING_CHAR);

    log_e("Asserting that we've appended the END_OF_VALUE and END_OF_STRING to the input string: %s -> %s", SWC_value_output,SWC_value_input);

}

void writing_and_reading_SWC(void)
{
    unsigned long start_time = millis();
    int nr_SWC_values = 1;
    int size_of_Varray = SWC_VALUE_ARRAY_SIZE;
    const char * test_path = "/SWC_w_r_test";

    overwrite_value_array(size_of_Varray,test_path);


    unsigned char* raw_SWC_values[] = {
            const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>("1.23")),
            const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>("45.67"))
    };

    unsigned char * Varray = (unsigned char *) malloc(sizeof(unsigned char) * size_of_Varray);



    for(int i = 0; i<nr_SWC_values; i++)
    {
        insert_at_carriage_return_and_save(test_path,raw_SWC_values[i%1], &format_SWC,SIZE_OF_SWC_AFTER_FORMATTING,size_of_Varray);
    }


    fileMan.load_file(test_path,Varray,size_of_Varray-1);
    log_e("%s",Varray);
    log_e("%lu",millis()-start_time);
}


void setup()
{
    delay(5000); // service delay


    //DON'T PUT ANYTHING BEFORE THIS EXCEPT FOR DELAY!!!!
    UNITY_BEGIN(); //Define stuff after this

    RUN_TEST(formating_stop_char);
    RUN_TEST(writing_and_reading_SWC);
    UNITY_END(); // stop unit testing
}

void loop()
{
}
