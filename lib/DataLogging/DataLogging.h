//
// Created by DripTooHard on 02-10-2023.
//

#ifndef PBT_DATALOGGING_H
#define PBT_DATALOGGING_H

#include <FileManager.h>
#include "FORMATTING_MACROS.h"
#include <cstdlib>

/*
 * Formats a string for being written in our value arrays
 *
 * With  END_OF_VALUE_CHAR representing the end of the string, and the index after that, the beginning of the new.
         END_OF_STRING_CHAR representing the end of the whole value array.
 */
unsigned char * format_value_for_FS(unsigned char * input,int input_size, int * output_size);

void create_value_array(int nr_chars, const char * path);

#endif //PBT_DATALOGGING_H
