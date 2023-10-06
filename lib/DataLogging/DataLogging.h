//
// Created by DripTooHard on 02-10-2023.
//

#ifndef PBT_DATALOGGING_H
#define PBT_DATALOGGING_H

#include <Effortless_SPIFFS.h>
#include <FileManager.h>
#include "FORMATTING_MACROS.h"
#include <cstdlib>

/*
 * Formats a string for being written in our value arrays
 *
 * With  END_OF_VALUE_CHAR representing the end of the string, and the index after that, the beginning of the new.
         END_OF_STRING_CHAR representing the end of the whole value array.
 */
unsigned char * format_value_for_FS(unsigned char * input,int input_size);

unsigned char * format_SWC(unsigned char* input);

unsigned char* long_to_char_array(long value);

unsigned char* format_long_to_char_array(unsigned char* value);

unsigned char* format_and_convert_long(long value);

unsigned char* format_fake(unsigned char*);

void overwrite_value_array(int nr_chars, const char * path);

void print_unsigned_char_array_as_long(const char* path,int data_size);

int find_carriage_return_index(const unsigned char* str, int size);

unsigned char * format_timestamp(unsigned char* long_uchar_array);

bool insert_at_carriage_return_and_save(const char* path, long insert_long, int insert_string_size, int value_array_size , int index, int * incrementer);


bool insert_at_carriage_return_and_save(const char* path, unsigned char * insert_long, int insert_string_size, int value_array_size , int index, int * incrementer);


long char_array_to_long(unsigned char* charArray);

unsigned char* format_value_for_FS(unsigned char *input, int input_size);

void overwrite_value_array(int nr_chars, const char * path);


#endif //PBT_DATALOGGING_H
