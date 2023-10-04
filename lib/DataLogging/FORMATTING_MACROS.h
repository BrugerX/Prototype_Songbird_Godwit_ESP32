//
// Created by DripTooHard on 02-10-2023.
//

#ifndef PBT_FORMATTING_MACROS_H
#define PBT_FORMATTING_MACROS_H

#define END_OF_VALUE_CHAR 44
#define END_OF_STRING_CHAR 11

#define SIZE_OF_FORMATTING_HEADER 2
#define SIZE_OF_SWC_AFTER_FORMATTING 6 //The combined size of the value and the indicating extra chars
#define SIZE_OF_TIMESTAMP_AFTER_FORMATTING 6

#define SWC_VALUE_ARRAY_SIZE 4*6*2880
#define TIMESTEP_VALUE_ARRAY_SIZE 4*6*2880

#define TIMESTEP_VALUE_ARRAY_PATH "\tstep_vray"
#define SWC_VALUE_ARRAY_PATH "\SWC_vray"

#endif //PBT_FORMATTING_MACROS_H
