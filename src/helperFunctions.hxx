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
#include <vtkMetaImageReader.h>
#include <vtkOBJReader.h>
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
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>

#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkProperty.h>
#include <vtkImageReslice.h>

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
*   Check the input arguements provided in the commandline when running the program.
*
*   @param   dicomFile   input DICOM directory
*   @param   objFile     input OBJ file
*
*   @returns a boolean representing whether the input arguements are valid
*            FALSE for invalid arguement
*            TRUE for a valid DICOM directory and valid OBJ file
*/
bool checkInputs( std::string dicomFile, std::string objFile );

/***************************************************************************/

#endif // HELPERFUNCTIONS_H