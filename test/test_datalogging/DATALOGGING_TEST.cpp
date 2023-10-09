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


void writing_and_reading_SWC(void)
{
    int nr_SWC_values = 2;
    int incrementer = 0;
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
        insert_at_carriage_return_and_save(test_path,raw_SWC_values[i],5,size_of_Varray,incrementer*5,&incrementer);
    }


    fileMan.load_file(test_path,Varray,size_of_Varray-1);

    log_e("%s",Varray);
    free(Varray);

}


void writing_and_reading_long(void)
{
    int nr_long_values = 4;
    int incrementer = 0;
    int size_of_Varray = TIMESTEP_VALUE_ARRAY_SIZE;
    const char * test_path = "/longwtest";


    //Getting the write values ready
    long raw_long_values[] = {0x41424344L, 0x41424344L,0x41424344L,0x41424344L};
    unsigned char * Varray = (unsigned char *) malloc(sizeof(unsigned char) * size_of_Varray);


    for(int i = 0; i < nr_long_values; i++)
    {


        bool success = insert_at_carriage_return_and_save(
                test_path,
                raw_long_values[i],
                SIZE_OF_TIMESTAMP_AFTER_TURNING_INTO_UCHAR,size_of_Varray,incrementer*4,&incrementer);

    }

    fileMan.load_file(test_path, Varray, size_of_Varray - 1);

    for(int i = 0; i<nr_long_values*4;i++)
    {
        log_e("%c",Varray[i]);
    }

}



void setup()
{
    delay(5000); // service delay


    //DON'T PUT ANYTHING BEFORE THIS EXCEPT FOR DELAY!!!!
    UNITY_BEGIN(); //Define stuff after this

    RUN_TEST(writing_and_reading_SWC);
    //RUN_TEST(turning_long_into_unchar);
    RUN_TEST(writing_and_reading_long);
    UNITY_END(); // stop unit testing
}

void loop()
{
}
