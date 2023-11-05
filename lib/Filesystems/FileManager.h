//
// Created by DripTooHard on 14-04-2023.
//

#ifndef FAGPROJEKTLORA2023_FILEMANAGER_H
#define FAGPROJEKTLORA2023_FILEMANAGER_H

#include "SPIFFS.h"
#include "SD.h"
#include "SPI.h"
#include "FS.h"
#include "FORMATTING_MACROS.h"



class FileManager{
public:
    FileManager(){}
    virtual ~FileManager(){}
    virtual bool save_file(const char * filePath, const unsigned char * dataToWrite) = 0;
    virtual bool load_file(const char * filePath, unsigned char * resultArray) = 0;
    virtual bool delete_file(const char * filePath) = 0;
    virtual bool exists(const char * filePath) = 0;
};


/**
 * Only a single SPIFFSFileManager can do operations with the flash at any given time
 * Once the previously active SPIFFSFileManager has dismounted, the next one must mount
 * The SPIFFSFileManager starts out mounted, given that no other filemanager is mounted
 * Currently it is only possible to mount to the same partition and with the same base path - will be added to the constructor and the mount() method when neccessary
 */

class SPIFFSFileManager{


private:

    fs::SPIFFSFS fileSystem;

    SPIFFSFileManager();

    ~SPIFFSFileManager();

public:


    static SPIFFSFileManager& get_instance() {
        static SPIFFSFileManager instance;
        return instance;
    }


    SPIFFSFileManager(const SPIFFSFileManager&) = delete;             // Delete copy constructor
    SPIFFSFileManager& operator=(const SPIFFSFileManager&) = delete;  // Delete copy assignment operator


    /**
     * Saves a file to the flash using SPIFFs
     *
     * \return true if a success, false if a failure, prints a custom error message
     */
    bool save_file(const char * filePath, const unsigned char * dataToWrite);

    bool exists(const char * filePath);

    bool delete_file(const char * filePath);

    bool save_file(const char* filePath, const unsigned char* dataToWrite, int dataSize);

    bool save_file_with_retries(const char *filePath, const unsigned char *dataToWrite,int size_of_file, int attempts, int mS_delay_between);

    /**
     * Warning: Do not use this with strings. It will append random characters at the end of the string array.
     * Instead use the load_file version, where you have to manually enter the ending index of the string.
     */
    bool load_file(const char * filePath, unsigned char * resultArray);

    /**
     *
     * Loads <= endIdx values from a file. Will automatically append \0 at the end of the array.
     *
     * @param filePath Filepath to load from, must begin with \
     * @param resultArray Where to load the array into
     * @param nrIndicesOfArray sizeOf(resultArray)/sizeOf(resultArray) or auto * array[n] => endIdx = n
     *                          |resultArray| >= endIdx
     *
     * @result resultArray now contains unsigned chars read from the file at filePath in the indices < endIdx,
     *         with resultArray[endIdx]
     */
    bool load_file(const char * filePath, unsigned char * resultArray, size_t endIdx);

    /**
     * Will dismount the current SPIFFSFileManager object from the SPIFFS, allowing another instance of the object to mount.
     * Read the README for more details.
     */
    void dismount();

    /**
    * Will mount the current SPIFFSFileManager object unto the SPIFFS, if no other SPIFFSFileManager is mounted.
    */
    bool mount();

};


class SDFileManager{


private:

    SDFileManager();

    ~SDFileManager();

public:


    static SDFileManager& get_instance() {
        static SDFileManager instance;
        return instance;
    }


    SDFileManager(const SDFileManager&) = delete;             // Delete copy constructor
    SDFileManager& operator=(const SDFileManager&) = delete;  // Delete copy assignment operator



    /**
     * Saves a file to the flash using SPIFFs
     *
     * \return true if a success, false if a failure, prints a custom error message
     */
    bool write_file(const char *filePath, const unsigned char *dataToWrite, size_t dataSize);

    bool append_file(const char * filePath, const unsigned char * dataToWrite);

    bool exists(const char * filePath);

    /**
    * Will mount the current SPIFFSFileManager object unto the SPIFFS, if no other SPIFFSFileManager is mounted.
    */
    void mount();

};


#endif //FAGPROJEKTLORA2023_FILEMANAGER_H
