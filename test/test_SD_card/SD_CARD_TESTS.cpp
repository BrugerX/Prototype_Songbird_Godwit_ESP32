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

SPIFFSFileManager& fileMan = SPIFFSFileManager::get_instance();

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


void setup()
{
    delay(5000); // service delay


    //DON'T PUT ANYTHING BEFORE THIS EXCEPT FOR DELAY!!!!
    UNITY_BEGIN(); //Define stuff after this
    RUN_TEST(test_mounting);
    UNITY_END(); // stop unit testing
}

void loop()
{
}
