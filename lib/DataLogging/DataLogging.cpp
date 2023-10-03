//
// Created by DripTooHard on 02-10-2023.
//

#include "DataLogging.h"

unsigned char* format_value_for_FS(unsigned char *input, int input_size,int * output_size) {
    int final_size = input_size + 2;
    unsigned char* formatted_input = (unsigned char*) malloc(final_size * sizeof(unsigned char));

    // Check if memory was allocated successfully

    for (int i = 0; i < input_size; i++) {
        formatted_input[i] = input[i];
    }

    formatted_input[input_size] = (unsigned char) END_OF_VALUE_CHAR;
    formatted_input[input_size + 1] = (unsigned char) END_OF_STRING_CHAR;

    *output_size = final_size;

    return formatted_input;
}

void save_value_array(int nr_chars, const char * path)
{

}