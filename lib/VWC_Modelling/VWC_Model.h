//
// Created by DripTooHard on 02-08-2023.
//

#ifndef PBT_VWC_MODEL_H
#define PBT_VWC_MODEL_H

class Regression_Model
{
public:
    // Define a type for the prediction function

    // Constructor taking a prediction function as a parameter
    Regression_Model(float * parameters)
    {

        parameters = parameters;
    }

    // Function to predict VWC using the provided prediction function
    float predict(float voltage);


private:
    // Function pointer to hold the prediction function
    float * parameters;
    float (prediction_function)(float voltage, float * parameters);
};


class VWC_predictor
        {


};

#endif //PBT_VWC_MODEL_H
