/****************************************************************************
*   helperFunctions.cxx
*
*   Created by:     Michael Kuczynski
*   Created on:     19/01/2019
*   Updated on:     06/03/2019
*   Version:        1.3
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
bool checkInputs( std::string dicomFile, std::string objFile )
{
    bool validDICOM = false;
    bool validOBJ = false;

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
    std::size_t dicomPeriod = dicomFile.find_last_of(".");
    std::size_t objPeriod   = objFile.find_last_of(".");

    /* 
    *   We can have potential problems here where a period is included in the file path.
    *   For this assignment, file extensions will only be ".dcm".
    *   Thus, we can say that if the third last character in the input is not a period, we likely have a directory.
    */
    if ( dicomPeriod == std::string::npos || ( dicomFile[dicomFile.length() - 4] ) != '.' )
    {
        // TO-DO: Find a way to detect if the provided directory is valid (on all systems...)

        // First input is a directory containing a DICOM series
        validDICOM = true;
        std::cout << "First input arguement is a directory. Checking contents of the directory... \n";
        std::cout << "Reading DICOM series from " << dicomFile << "... \n";
    }
    else if ( dicomPeriod != std::string::npos )
    {
        // Input is a file. Check filetype.
        std::cout << "Input provided is a file. Checking the filetype... \n";
        
        std::string fileExtension;
        fileExtension.assign(dicomFile, dicomPeriod, 4);

        if ( fileExtension == ".dcm" )
        {
            validDICOM = true;
        }
        else
        {
            validDICOM = false;
        }
    }
    else
    {
        // Only accept DICOM filetypes.
        validDICOM = false;
        std::cout << "ERROR: Incorrect input arguement. Please provide a valid DICOM directory. \n";
    }

    /*
    *   Check the input OBJ file to make sure its the correct filetype.
    *   Use the same method as checking valid DICOM directories above.
    */
    if ( objPeriod != std::string::npos || ( objFile[objFile.length() - 4] ) == '.' ) 
    {
        // Second input is an OBJ file
        std::string fileExtension;
        fileExtension.assign( objFile, objPeriod, 4);

        if ( fileExtension == ".obj" )
        {
            validOBJ = true;
            std::cout << "Reading OBJ file " << objFile << "... \n";
        }
        else
        {
            validOBJ = false;
            std::cout << "ERROR: Incorrect input arguement. Please provide a valid OBJ file or directory. \n";
        }
    }

    return ( validDICOM * validOBJ );
}
/***************************************************************************/