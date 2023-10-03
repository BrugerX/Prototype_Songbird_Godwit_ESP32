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

#define size_of_premade_string 8

char * premade_string_to_write = "12.34";

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
    int output_size = 0;

    unsigned char * SWC_value_output = format_value_for_FS(reinterpret_cast<unsigned char *>(SWC_value_input),size_of_SWC_value_input, &output_size);

    for(int i = 0; i<size_of_SWC_value_input;i++)
    {
        TEST_ASSERT_EQUAL((unsigned char) SWC_value_input[i], SWC_value_output[i]);
    }

    TEST_ASSERT_TRUE(SWC_value_output[size_of_SWC_value_input] == END_OF_VALUE_CHAR);
    TEST_ASSERT_TRUE(SWC_value_output[size_of_SWC_value_input+1] == END_OF_STRING_CHAR);

    log_e("Asserting that we've appended the END_OF_VALUE and END_OF_STRING to the input string: %s -> %s", SWC_value_output,SWC_value_input);

    TEST_ASSERT_EQUAL(size_of_SWC_value_input + 2,output_size);
}


void setup()
{
    delay(5000); // service delay


    //DON'T PUT ANYTHING BEFORE THIS EXCEPT FOR DELAY!!!!
    UNITY_BEGIN(); //Define stuff after this

    RUN_TEST(formating_stop_char);
    UNITY_END(); // stop unit testing
}

void loop()
{
}
