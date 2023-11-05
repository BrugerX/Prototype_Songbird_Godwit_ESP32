//
// Created by DripTooHard on 03-10-2023.
//

#include <DataLogging.h>
#include <TestUtility.h>

//
// Created by DripTooHard on 20-05-2023.
//


#include <Arduino.h>
#include <unity.h>
#include <DataLogging.h>
#include <UnsignedStringUtility.h>

#define size_of_premade_string 8

SDFileManager& fileMan = SDFileManager::get_instance();

void setUp(void) {
    // set stuff up here
}


void tearDown(void) {
    // delete stuff down here
}



void create_SWC_tStamp_tuple_test(void)
{
    // The float as a string
    unsigned char float_str[] = "12.12";
    // The unsigned long value
    unsigned long ulong_value = 123456789UL; // Example value
    auto * correct_value =(unsigned char*) "(12.12,123456789)";

    unsigned char * result_tuple = create_SWC_tStamp_tuple(float_str,ulong_value);
    log_e("%s",result_tuple);


    free(result_tuple);


}

void create_and_save_SWC_tStamp_tuple(void)
{
    // The float as a string
    unsigned char float_str[] = "12.12";
    // The unsigned long value
    unsigned long ulong_value = 123456789UL; // Example value
    auto * correct_value =(unsigned char*) "(12.12,123456789)";

    unsigned char * result_tuple = create_SWC_tStamp_tuple(float_str,ulong_value);
    log_e("%s",result_tuple);

    fileMan.write_file("/test.txt",result_tuple,strlen((const char*)result_tuple));
    free(result_tuple);
}

void turning_long_into_unchar(void)
{
    long long_value = ('T' << 24) | ('e' << 16) | ('s' << 8) | 't';
    unsigned char * long_uchar_array = long_to_char_array(long_value);
    log_e("%s",long_uchar_array);
    TEST_ASSERT_EQUAL((unsigned char)'T',long_uchar_array[0]);
    TEST_ASSERT_EQUAL((unsigned char)'e',long_uchar_array[1]);
    TEST_ASSERT_EQUAL((unsigned char)'s',long_uchar_array[2]);
    TEST_ASSERT_EQUAL((unsigned char)'t',long_uchar_array[3]);
}







void setup()
{
    delay(5000); // service delay


    //DON'T PUT ANYTHING BEFORE THIS EXCEPT FOR DELAY!!!!
    UNITY_BEGIN(); //Define stuff after this
    fileMan.mount();
    RUN_TEST(create_SWC_tStamp_tuple_test);
    RUN_TEST(create_and_save_SWC_tStamp_tuple);
    UNITY_END(); // stop unit testing
}

void loop()
{
}
