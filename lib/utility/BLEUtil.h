//
// Created by DripTooHard on 25-07-2023.
//

#ifndef PBT_BLEUTIL_H
#define PBT_BLEUTIL_H


#include "NimBLEDevice.h"

bool isSameCharacteristic(const char * UUID, NimBLECharacteristic * bleCharacteristic);

#endif //PBT_BLEUTIL_H
