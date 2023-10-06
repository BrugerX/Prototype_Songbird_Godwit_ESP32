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

    log_e("Formatted output: %s",formatted_input);
    return formatted_input;
}

unsigned char* long_to_char_array(long value) {
    unsigned char* charArray = (unsigned char*)malloc(4 * sizeof(unsigned char));
    // Extract each byte and store it in the char array
    charArray[0] = (unsigned char) (value >> 24) & 0xFF;
    charArray[1] = (unsigned char) (value >> 16) & 0xFF;
    charArray[2] = (unsigned char) (value >> 8)  & 0xFF;
    charArray[3] = (unsigned char)  value        & 0xFF;
    return charArray;
}


long char_array_to_long(unsigned char* charArray) {
    return ((long)charArray[0] << 24) |
           ((long)charArray[1] << 16) |
           ((long)charArray[2] << 8)  |
           (long)charArray[3];
}

unsigned char * format_timestamp(unsigned char* long_uchar_array)
{
    return format_value_for_FS(long_uchar_array,SIZE_OF_TIMESTAMP_AFTER_TURNING_INTO_UCHAR);
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

bool insert_at_carriage_return_and_save(const char* path, unsigned char * input_string, int insert_string_size, int value_array_size , int index, int * incrementer) {
    SPIFFSFileManager& fileMan = SPIFFSFileManager::get_instance();

    //Load the string
    unsigned char * value_array = (unsigned char *) malloc(sizeof(unsigned char )*value_array_size);
    fileMan.load_file(path,value_array,value_array_size -1);

    //Write to the string
    for(int i = 0;i<insert_string_size;i++)
    {
        value_array[index + i] = input_string[i];
    }

    int success = 0;
    int retries = 0;

    while (retries < MAX_RETRIES) {
        // Try to save the file
        success = fileMan.save_file(path, value_array);

        // Check if save was successful
        if (success) {
            printf("File saved successfully.\n");
            break;
        } else {
            printf("Failed to save the file. Retrying in 0.5 seconds...\n");
            vTaskDelay(500 / portTICK_PERIOD_MS); // 0.5s delay
            retries++;
        }
    }
    //free value array
    free(value_array);

    incrementer += 1;

    return true;
}

bool insert_at_carriage_return_and_save(const char* path, long insert_long, int insert_string_size, int value_array_size , int index, int * incrementer) {
    SPIFFSFileManager& fileMan = SPIFFSFileManager::get_instance();

    //First convert the long to a char
    unsigned char * input_string = long_to_char_array(insert_long);

    //Load the string
    unsigned char * value_array = (unsigned char *) malloc(sizeof(unsigned char )*value_array_size);
    fileMan.load_file(path,value_array,value_array_size -1);

    //Write to the string
    for(int i = 0;i<insert_string_size;i++)
    {
        value_array[index + i] = input_string[i];
    }

    //Save the string
    fileMan.save_file(path,value_array);

    //free value array
    free(value_array);

    (*incrementer)++;

    return true;
}

