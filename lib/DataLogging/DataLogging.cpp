//
// Created by DripTooHard on 02-10-2023.
//

#include "DataLogging.h"

unsigned char* format_value_for_FS(unsigned char *input, int input_size) {
    int final_size = input_size + 2;
    unsigned char* formatted_input = (unsigned char*) malloc(final_size * sizeof(unsigned char));

    // Check if memory was allocated successfully

    for (int i = 0; i < input_size; i++) {
        formatted_input[i] = input[i];
    }

    formatted_input[input_size] = (unsigned char) END_OF_VALUE_CHAR;
    formatted_input[input_size + 1] = (unsigned char) END_OF_STRING_CHAR;


    return formatted_input;
}

unsigned char* long_to_char_array(long value) {
    unsigned char* result = (unsigned char*)malloc(4 * sizeof(unsigned char));
    if(result != nullptr) {
        for(int i = 0; i < 4; i++) {
            result[i] = (value >> (8 * i)) & 0xFF;
        }
    }
    return result;
}


unsigned char* format_and_convert_long(long value) {
    unsigned char* char_array = long_to_char_array(value);
    if(char_array == nullptr) {
        return nullptr;
    }
    unsigned char* formatted_array = format_value_for_FS(char_array, 4);
    free(char_array);
    return formatted_array;
}

unsigned char * format_SWC(unsigned char* input) {
    unsigned char output[5];

    // Ensure output is cleared/initialized
    for(int i = 0; i < 5; i++) {
        output[i] = 0;
    }

    if(input[1] == '.') {  // Format is "X.XX"
        output[0] = '0';
        output[1] = input[0];
        output[2] = input[1];
        output[3] = input[2];
        output[4] = input[3];
    } else { // Assumes the format is "XX.XX"
        output[0] = input[0];
        output[1] = input[1];
        output[2] = input[2];
        output[3] = input[3];
        output[4] = input[4];
    }

    // Note: The implementation of format_value_for_FS is unknown from this snippet.
    // Ensure its definition is correct and does handle memory management well.
    return format_value_for_FS(output, 5);
    // Assuming third parameter is needed based on provided snippet in previous messages.
}


int find_carriage_return_index(const unsigned char* str, int size) {
    int i = 0;
    for(int j = 0; j<size;j++) {
        if(str[i] == END_OF_STRING_CHAR) { // Using the macro here
            return i;
        }
        i++;
    }
    return -1;  // END_OF_STRING_CHAR not found
}



void overwrite_value_array(int nr_chars, const char * path)
{
    SPIFFSFileManager& fileManager = SPIFFSFileManager::get_instance();
    unsigned char * value_array = (unsigned char *) malloc(sizeof(char)*nr_chars);
    value_array[0] = END_OF_STRING_CHAR;
    value_array[nr_chars-1] = '\0';
    fileManager.save_file(path,value_array);
    free(value_array);
}

void print_unsigned_char_array_as_long(const char* path,int data_size) {
    SPIFFSFileManager& fileManager = SPIFFSFileManager::get_instance();

    if(!fileManager.exists(path)) {
        printf("Error: File does not exist\n");
        return;
    }


    unsigned char* data = (unsigned char*)malloc(data_size + 1);  // +1 for null-terminator
    if(data == nullptr) {
        printf("Error allocating memory\n");
        return;
    }

    if(!fileManager.load_file(path, data)) {
        printf("Error loading file\n");
        free(data);
        return;
    }

    if(data_size % 4 != 0) {
        printf("Warning: Data size is not a multiple of 4, last bytes will be ignored\n");
    }

    for(size_t i = 0; i < data_size; i+=4) {
        if(i + 3 < data_size) {
            long value =  (long)data[i]       |
                          ((long)data[i + 1] << 8)  |
                          ((long)data[i + 2] << 16) |
                          ((long)data[i + 3] << 24);
            printf("Long value: %ld\n", value);
        }
    }

    free(data);
}

bool insert_at_carriage_return_and_save(const char* path, unsigned char* insert_str, unsigned char* (*format_func)(unsigned char*), int insert_str_size, int value_array_size) {
    SPIFFSFileManager& fileManager = SPIFFSFileManager::get_instance();

    // Load the string from the file.
    unsigned char *loaded_str = (unsigned char *) malloc(sizeof(unsigned char)*value_array_size);
    fileManager.load_file(path,loaded_str,value_array_size-1);  // Assume load_file function exists and returns dynamically allocated memory.

    if(loaded_str == nullptr) {
        return false;  // Error handling: file couldn't be loaded.
    }

    int carriage_return_index = find_carriage_return_index(loaded_str,value_array_size);

    if( carriage_return_index == value_array_size -1)
    {
        log_e("Tried writing value array, but the value array is full");
        throw std::runtime_error("ARRAY IS FULL");
    }

    if(carriage_return_index == -1) {
        free(loaded_str);
        log_e("Cannot find END_OF_STRING_CHAR, sting: %s",loaded_str);
        return false;  // Error handling: "\r" not found.
    }

    int insert_str_formatted_size = insert_str_size + SIZE_OF_FORMATTING_HEADER;
    unsigned char* formatted_insert_str = format_func(insert_str);
    if(formatted_insert_str == nullptr) {
        free(loaded_str);
        return false;  // Error handling: formatting failed.
    }


    // Create a new string to store the merged content.
    unsigned char* merged_str = (unsigned char*)malloc(strlen((char*)loaded_str) + insert_str_formatted_size + 1);
    if(merged_str == nullptr) {
        free(loaded_str);
        free(formatted_insert_str);
        return false;  // Error handling: memory allocation failed.
    }

    // Merge the strings.
    memcpy(merged_str, loaded_str, carriage_return_index);
    memcpy(merged_str + carriage_return_index, formatted_insert_str, insert_str_formatted_size);
    strcpy((char*)(merged_str + carriage_return_index + insert_str_formatted_size), (char*)(loaded_str + carriage_return_index));

    // Save the new string back to the file.
    fileManager.save_file(path, merged_str);  // Assume save_file function exists.

    // Free all dynamically allocated memory.
    free(loaded_str);
    free(formatted_insert_str);
    free(merged_str);

    return false;
}
