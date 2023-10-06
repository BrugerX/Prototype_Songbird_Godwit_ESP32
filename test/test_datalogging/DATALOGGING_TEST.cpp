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

void turning_long_into_unchar_and_formatting(void)
{

    long long_value = ('T' << 24) | ('e' << 16) | ('s' << 8) | 't';
    unsigned char * unformatted_long_uchar_array = long_to_char_array(long_value);
    unsigned char * formatted_long_uchar_array = format_timestamp(unformatted_long_uchar_array);

    log_e("%s",formatted_long_uchar_array);
    TEST_ASSERT_EQUAL((unsigned char)'T',formatted_long_uchar_array[0]);
    TEST_ASSERT_EQUAL((unsigned char)'e',formatted_long_uchar_array[1]);
    TEST_ASSERT_EQUAL((unsigned char)'s',formatted_long_uchar_array[2]);
    TEST_ASSERT_EQUAL((unsigned char)'t',formatted_long_uchar_array[3]);
    TEST_ASSERT_EQUAL((unsigned char) END_OF_VALUE_CHAR,formatted_long_uchar_array[4]);
    TEST_ASSERT_EQUAL((unsigned char) END_OF_STRING_CHAR,formatted_long_uchar_array[5]);
}

void writing_and_reading_SWC(void)
{
    int nr_SWC_values = 2;
    int size_of_Varray = nr_SWC_values*6+2;
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
    free(Varray);

}


void writing_and_reading_long(void)
{
    int nr_long_values = 2;
    int size_of_Varray = nr_long_values * SIZE_OF_TIMESTAMP_AFTER_FORMATTING + 9;
    const char * test_path = "/longwtest";

    //Initializing value array on flash
    overwrite_value_array(size_of_Varray, test_path);

    //Getting the write values ready
    long raw_long_values[] = {12345678L, 87654321L};
    unsigned char * Varray = (unsigned char *) malloc(sizeof(unsigned char) * size_of_Varray);


    for(int i = 0; i < nr_long_values; i++)
    {

        bool success = insert_at_carriage_return_and_save(
                test_path,
                raw_long_values[i],
                &format_timestamp,
                SIZE_OF_TIMESTAMP_AFTER_TURNING_INTO_UCHAR,
                size_of_Varray
        );


        if(!success)
        {
            log_e("Insertion and save operation failed");
            free(Varray);
            return;
        }
    }

    if(!fileMan.load_file(test_path, Varray, size_of_Varray - 1))
    {
        log_e("Loading file failed");
        free(Varray);
        return;
    }

    log_e("%s", Varray);

}



void setup()
{
    delay(5000); // service delay


    //DON'T PUT ANYTHING BEFORE THIS EXCEPT FOR DELAY!!!!
    UNITY_BEGIN(); //Define stuff after this

    //RUN_TEST(formating_stop_char);
    //RUN_TEST(writing_and_reading_SWC);
    RUN_TEST(turning_long_into_unchar);
    //RUN_TEST(turning_long_into_unchar_and_formatting);
    RUN_TEST(writing_and_reading_long);
    UNITY_END(); // stop unit testing
}

void loop()
{
}
