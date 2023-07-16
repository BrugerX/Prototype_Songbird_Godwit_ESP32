//
// Created by DripTooHard on 01-07-2023.
//

#include "BatteryPower.h"
/*
BatteryManager::BatteryManager() {
    log_e("Wire");
    //two_wire->begin(21,22);
    Wire.begin(21,22);


    log_e("Wired");

    if(axp->begin(Wire,AXP192_SLAVE_ADDRESS) == AXP_FAIL)
    {
        log_e("ERROR: COULD NOT INITIATE AXP20");
        throw std::runtime_error("COULD NOT INITIATE AXP20");
    }

    log_e("Done instnatiati");

}

int BatteryManager::get_btri_power_prctg()
{
    int res = axp->getBattPercentage();

    //axp20x's getBattPercentage returns 0 if the device isn't connected to a battery
    if(res == 0)
    {
        throw std::logic_error("DEVICE IS NOT CONNECTED TO A BATTERY");
    }
    else if (res == AXP_NOT_SUPPORT)
    {
        throw std::logic_error("DEVICE DOES NOT SUPPORT AXP20");
    }

    return res;
}*/