//
// Created by DripTooHard on 01-07-2023.
//

#ifndef PROTOBLETEST_BATTERYPOWER_H
#define PROTOBLETEST_BATTERYPOWER_H

#include <Wire.h>
#include "axp20x.h"

class BatteryManager{


private:
    AXP20X_Class * axp = new AXP20X_Class();
    TwoWire * two_wire = new TwoWire(0);

public:
    int get_btri_power_prctg();

    BatteryManager();
};


#endif //PROTOBLETEST_BATTERYPOWER_H
