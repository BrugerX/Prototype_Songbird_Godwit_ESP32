//
// Created by DripTooHard on 14-04-2023.
//

#include "FileManager.h"



class FileManager{
public:
    FileManager(){}
    virtual ~FileManager(){}
    virtual bool save_file(const char * filePath, const unsigned char * dataToWrite) = 0;
    virtual bool load_file(const char * filePath, unsigned char * resultArray) = 0;
    virtual bool delete_file(const char * filePath) = 0;
    virtual bool exists(const char * filePath) = 0;
};


SPIFFSFileManager::SPIFFSFileManager()
{

}


SPIFFSFileManager::~SPIFFSFileManager(){
}

bool SPIFFSFileManager::save_file(const char *filePath, const unsigned char *dataToWrite) {
    log_e("Writing file: %s\r\n", filePath);

    File file = fileSystem.open((const char *)filePath, FILE_WRITE);

    //Failed to open
    if(!file){
        log_e("− failed to open file for writing");
        throw std::logic_error("− failed to open file for writing");
    }

    //Written succesfully
    size_t write_result = file.print( (const char *) dataToWrite);

    if(write_result){
        log_e("− file written");
        file.close();
        return true;
    }
    else //Failed to write
    {
        file.close();
        log_e("- Write failed");
        return false;
    }

}

bool SPIFFSFileManager::save_file(const char* filePath, const unsigned char* dataToWrite, int dataSize) {
    log_e("Writing file: %s\r\n", filePath);

    File file = fileSystem.open((const char *) filePath, FILE_WRITE);

    // Failed to open
    if (!file) {
        log_e("− failed to open file for writing");
        throw std::logic_error("− failed to open file for writing");
    }

    // Written successfully
    size_t write_result = file.write(dataToWrite, dataSize);

    if (write_result == dataSize) {
        log_e("− file written");
        file.close();
        return true;
    }
        // Failed to write
    else {
        file.close();
        log_e("- Write failed");
        return false;
    }
}

bool SPIFFSFileManager::save_file_with_retries(const char *filePath, const unsigned char *dataToWrite,int size_of_file, int attempts, int mS_delay_between) {
    int success = 0;
    int retries = 0;

    SPIFFSFileManager& fileMan = SPIFFSFileManager::get_instance();

    while (retries < attempts) {
        // Try to save the file
        success = fileMan.save_file(filePath, dataToWrite,size_of_file);

        // Check if save was successful
        if (success) {
            log_i("Succesfully inserted value into value array.\n");
            return true;
        } else {
            log_i("Failed to insert value array into path %s for the %i 'th time. Retrying again in %i milliseconds",filePath,retries,mS_delay_between);
            vTaskDelay(mS_delay_between / portTICK_PERIOD_MS);
            retries++;
        }
    }

    return false;
}


bool SPIFFSFileManager::exists(const char *filePath) {
    return fileSystem.exists((const char *) filePath);
}

bool SPIFFSFileManager::delete_file(const char * filePath){
    Serial.printf("Deleting file: %s\r\n", filePath);
    if(fileSystem.remove((const char *)filePath)){
        Serial.println("− file deleted");
        return true;
    }
    else
    {
        throw std::logic_error("− delete failed");
    }
}

bool SPIFFSFileManager::load_file(const char * filePath, unsigned char * resultArray){
    File f1 = fileSystem.open(filePath);
    if(!f1 || f1.isDirectory()){
        log_e("SPIFFSFileManager: Failed to open file %s. You possibly tried to load a directory",filePath);
        throw std::logic_error("− failed to open file for reading. You possibly tried to load a directory");
    }

    int i = 0;
    unsigned char res;

    while(f1.available()){
        res = f1.read();
        resultArray[i++] = res;
    }

    return true;



}

bool SPIFFSFileManager::load_file(const char * filePath, unsigned char * resultArray, size_t endIdx){
    File f1 = fileSystem.open(filePath,FILE_READ);
    if(!f1 || f1.isDirectory()){
        throw std::logic_error("− failed to open file for reading. You possibly tried to load a directory");
    }

    unsigned char res;

    for (int i = 0; i<endIdx; i++){
        if(!f1.available()){
            break;
        }
        res = f1.read();
        resultArray[i] = res;
    }

    resultArray[endIdx] = '\0';

    return true;

}

void SPIFFSFileManager::dismount() {
    fileSystem.end();
}

//TODO: Default arguments ift. basepath + partition label
bool SPIFFSFileManager::mount() {
    if(!fileSystem.begin(true)){
        log_e("SPIFFS failed to mount");
        throw std::logic_error("SPIFFS failed to mount");
    }

    return true;
}