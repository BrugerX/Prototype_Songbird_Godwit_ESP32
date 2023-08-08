//
// Created by DripTooHard on 02-08-2023.
//

#include "VWC_Model.h"


float Regression_Model:: predict(float voltage)
{
    // Call the prediction function through the function pointer
    return prediction_function(voltage, this->parameters);
}

float Regression_Model::prediction_function(float voltage, float *parameters) {
    return voltage*voltage*parameters[0] + voltage*parameters[1] + parameters[2];
}
