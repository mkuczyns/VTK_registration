/****************************************************************************
*   helperFunctions.cxx
*
*   Created by:     Michael Kuczynski
*   Created on:     19/01/2019
*   Version:        1.2
*   Description:    Implementation of additional classes and functions
*                   used by the main program.
****************************************************************************/

#include "helperFunctions.hxx"

/****************** Helper class "ImageMessage" functions ******************/
std::string ImageMessage::sliceNumberFormat( int minSlice, int maxSlice ) 
{
    std::stringstream tmp;
    tmp << "Slice Number:  " << minSlice + 1 << "/" << maxSlice + 1;
    return tmp.str();
}

std::string ImageMessage::windowLevelFormat( int windowLevel ) 
{
    std::stringstream tmp;
    tmp << "Window Level:  " << windowLevel;
    return tmp.str();
}

std::string ImageMessage::windowFormat( int window ) 
{
    std::stringstream tmp;
    tmp << "Window:  " << window;
    return tmp.str();
}
/***************************************************************************/

/************************* Other helper functions **************************/
fileType_t checkInputs( std::string inputFile )
{
    fileType_t fileType;

    /* 
    *   Attempt to find a period in the input arguement that seperates filename and filetype.
    *   Attempt to find a slash as well to determine if the input is a directory path.
    * 
    *   If no period exists or we have a slash, we have a directory name or path. 
    *   Otherwise check the file extension for the filetype.
    * 
    *   This may not be the best way to do this, but it should work on all systems.
    *   There are ways to check if a variable contains a file or directory, but they are specific to
    *   certain operating systems...
    */
    std::size_t period_Index = inputFile.find_last_of(".");

    /* 
    *   We can have potential problems here where a period is included in the file path.
    *   For this assignment, file extensions will only be ".dcm" or ".nii".
    *   Thus, we can say that if the third last character in the input is not a period, we likely have a directory.
    */
    if ( period_Index == std::string::npos || ( inputFile[inputFile.length() - 4] ) != '.' )
    {
        // Input is a directory containing a DICOM series
        fileType.type = 0;
        std::cout << "Input provided is a directory. Checking contents of the directory..." << std::endl;
        std::cout << "Reading DICOM series..." << std::endl;
    }
    else if ( period_Index != std::string::npos )
    {
        // Input is a file. Check filetype.
        std::cout << "Input provided is a file. Checking the filetype..." << std::endl;
        
        std::string fileExtension;
        fileExtension.assign(inputFile, period_Index, 4);

        if ( fileExtension == ".dcm" )
        {
            fileType.type = -1;
        }
        else if ( fileExtension == ".nii" )
        {
            fileType.type = 1;
            std::cout << "Reading NIfTI image..." << std::endl;
        }
        else
        {
            // Only accept DICOM or NIfTI filetypes.
            fileType.type = -1;
        }
    }

    return fileType;
}



int clamp( int value, int low = -1, int high = 1 )
{
    return std::min( std::max(value, low), high );
} 



void setThreshold( int tissueType, int& lower, int& upper )
{
    switch ( tissueType )
    {
        case -1:        // Invalid tissue input
        {
            break;
        }

        case 0:         // Bone
        {
            lower = 200;
            upper = 5000;
            break;
        }

        case 1:         // Fat
        {
            lower = 200;
            upper = 600;
            break;
        }

        case 2:         // Muscle
        {
            lower = 300;
            upper = 500;
            break;
        }

        case 3:         // Air
        {
            lower = 0;
            upper = 100;
            break;
        }

        default:        // Default case
        {
            break;
        }
    }
}
/***************************************************************************/