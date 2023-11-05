//
// Created by DripTooHard on 03-10-2023.
//

#include <FileManager.h>

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


void test_mounting(void)
{
    SDFileManager& sdMan = SDFileManager::get_instance();
    sdMan.mount();
}



void test_existing(void)
{
    const char * filePath = "/testExistingFP";
    TEST_ASSERT_FALSE(fileMan.exists(filePath));
    fileMan.write_file(filePath,(unsigned char *)"abc",3);
    TEST_ASSERT_TRUE(fileMan.exists(filePath));
    fileMan.delete_file(filePath);
}

void test_writing_creates_files(void)
{

    auto * correct_value =(unsigned char*) "(12.12,123456789)";
    const char * filePath = "/testWritingCreatesFilesFP.txt";

    TEST_ASSERT_FALSE(fileMan.exists(filePath));

    fileMan.write_file(filePath,correct_value,strlen((const char*)correct_value));

    TEST_ASSERT_TRUE(fileMan.exists(filePath));

    fileMan.delete_file(filePath);

}

void test_append_creates_file(void)
{
    const char * filePath = "/testAppendingCreatesFP.txt";
    unsigned char * dataToWrite = (unsigned char *)"abc";
    TEST_ASSERT_FALSE(fileMan.exists(filePath));
    fileMan.append_file(filePath,dataToWrite,strlen((const char*)dataToWrite));
    TEST_ASSERT_TRUE(fileMan.exists(filePath));


    fileMan.delete_file(filePath);
}


void test_append_file(void)
{

    const char * filePath = "/testAppendingFP.txt";
    unsigned char * dataToWrite = (unsigned char *)"abc";

    TEST_ASSERT_FALSE(fileMan.exists(filePath));
    fileMan.write_file(filePath,dataToWrite, strlen((const char*)dataToWrite));
    TEST_ASSERT_TRUE(fileMan.exists(filePath));

    fileMan.append_file(filePath,dataToWrite, strlen((const char*)dataToWrite));
    fileMan.append_file(filePath,dataToWrite, strlen((const char*)dataToWrite));


    fileMan.delete_file(filePath);
}

void test_file_doesnt_exist_after_deletion(void)
{

    const char * filePath = "/testFileDoesntExistAfterDeletion.txt";
    unsigned char * dataToWrite = (unsigned char *)"abc";
    TEST_ASSERT_FALSE(fileMan.exists(filePath));
    fileMan.write_file(filePath,dataToWrite,strlen((const char*)dataToWrite));
    TEST_ASSERT_TRUE(fileMan.exists(filePath));
    fileMan.delete_file(filePath);
    TEST_ASSERT_FALSE(fileMan.exists(filePath));

}

void setup()
{
    delay(5000); // service delay


    //DON'T PUT ANYTHING BEFORE THIS EXCEPT FOR DELAY!!!!
    UNITY_BEGIN(); //Define stuff after this
    RUN_TEST(test_mounting);
    RUN_TEST(test_existing);
    RUN_TEST(test_writing_creates_files);
    RUN_TEST(test_append_creates_file);
    RUN_TEST(test_append_file);
    RUN_TEST(test_file_doesnt_exist_after_deletion);
    UNITY_END(); // stop unit testing
}

void loop()
{
}
