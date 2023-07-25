//
// Created by DripTooHard on 25-07-2023.
//

#include "BLEUtil.h"


bool isSameCharacteristic(const char * UUID, NimBLECharacteristic * bleCharacteristic)
{
    std::string characteristics_UUID = bleCharacteristic->getUUID();
    for(int i = 0; i<4;i++)
    {
        if(UUID[i] != characteristics_UUID[i+2])
        {
            return false;
        }
    }

    return true;
}