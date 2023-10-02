//
// Created by DripTooHard on 02-10-2023.
//

#ifndef PBT_DATALOGGING_H
#define PBT_DATALOGGING_H

#include "FORMATTING_MACROS.h"
#include <cstdlib>

/*
 * Formats a string for being written in our value arrays
 */
unsigned char * format_value_for_FS(unsigned char * input,int input_size);

#endif //PBT_DATALOGGING_H
