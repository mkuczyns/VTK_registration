/****************************************************************************
*   helperFunctions.hxx
*
*   Created by:     Michael Kuczynski
*   Created on:     19/01/2019
*   Description:    Definition of additional classes and functions
*                   used by the main program.
****************************************************************************/

#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <chrono>

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkImageMapper.h>

#include <vtkImageViewer2.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkDICOMImageReader.h>
#include <vtkNIFTIImageReader.h>
#include <vtkInteractorStyleImage.h>

#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>

#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkImageAlgorithm.h>

#include <vtkImageGaussianSmooth.h>

#include <vtkMarchingCubes.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageThreshold.h>

#include <vtkVolume.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkMetaImageReader.h>

/* 
*   A helper class to display messages with information about current slice
*   number and current window level in the rendered window.
*/
class ImageMessage 
{
    public:
        /*
        *   Create a message that shows the current slice number.
        *
        *   @param   minSlice   The minimum slice number
        *   @param   maxSlice   The maximum slice number
        *
        *   @returns A string steam with the slice message
        */
        static std::string sliceNumberFormat( int minSlice, int maxSlice );

        /*
        *   Create a message that shows the current window level.
        *
        *   @param   windowLevel   The current window level
        *
        *   @returns A string steam with the window level message
        */
        static std::string windowLevelFormat( int windowLevel );

        /*
        *   Create a message that shows the current window.
        *
        *   @param   window   The current window
        *
        *   @returns A string steam with the window message
        */
        static std::string windowFormat( int window );
};

/************************* Other helper functions **************************/

/*
*   Global threshold variables. Values will change depending on
*   the specified input tissue type to segment.
*/
extern int upperThreshold, lowerThreshold;

int clamp( int value );

struct fileType_t
{
    int type;
    fileType_t( int _type ) : type{ clamp( _type ) } {}
    fileType_t() : type{ -1 } {}
};

/*
*   Check the input arguements provided in the commandline when running the program.
*
*   @param   inputArguement   Input arguement from commandline
*
*   @returns the integer corresponding to the input type
*            -1 for invalid arguement
*             0 for a DICOM directory
*             1 for a NIfTI file
*/
fileType_t checkInputs( std::string inputFile );

/***************************************************************************/

#endif // HELPERFUNCTIONS_H