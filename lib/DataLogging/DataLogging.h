//
// Created by DripTooHard on 02-10-2023.
//

#ifndef PBT_DATALOGGING_H
#define PBT_DATALOGGING_H

#include <Effortless_SPIFFS.h>
#include <FileManager.h>
#include "FORMATTING_MACROS.h"
#include <cstdlib>


unsigned char * format_SWC(unsigned char* input);

unsigned char* long_to_char_array(long value);

void overwrite_value_array(int nr_chars, const char * path);

void print_unsigned_char_array_as_long(const char* path,int data_size);

bool insert_at_carriage_return_and_save(const char* path, long insert_long, int insert_string_size, int value_array_size , int index, int * incrementer);


bool insert_at_carriage_return_and_save(const char* path, unsigned char * insert_long, int insert_string_size, int value_array_size , int index, int * incrementer);


long char_array_to_long(unsigned char* charArray);

void overwrite_value_array(int nr_chars, const char * path);


#endif //PBT_DATALOGGING_H
