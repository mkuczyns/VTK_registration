/****************************************************************************
*   vtkRegistration.cxx
*
*   Created by:     Michael Kuczynski
*   Created on:     05/03/2019
*   Version:        1.0
*   Description:    
****************************************************************************/

#include "interactorStyler.hxx"

#include <itkVTKImageToImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include "itkOtsuMultipleThresholdsImageFilter.h"

vtkStandardNewMacro(myInteractorStyler);

int main(int argc, char* argv[])
{
  // Check if the provided input arguement is a file or directory.
  // Assign the correct value to tissueType variable.
  if ( argc != 2 )
  {
      std::cout << "ERROR: Incorrect program usage." << std:: endl;
      std::cout << "Correct usage: " << argv[0] << " <DICOM_Folder_Directory>" << std::endl;
      return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkImageData> volume = vtkSmartPointer<vtkImageData>::New();

  vtkSmartPointer<vtkDICOMImageReader> DICOM_reader;
  vtkSmartPointer<vtkNIFTIImageReader> NIFTI_reader;

  fileType_t inputFiletype( checkInputs( argv[1] ) );

  // Create a variable for the input arguement (if valid type).
  std::string inputFile = "";

  if ( inputFiletype.type == 0 || inputFiletype.type == 1)
  {
      inputFile = argv[1];
  }

  // Assign values to appropriate variables depending on if we have a DICOM series or NIfTI file.
  if ( inputFiletype.type == -1 )
  {
      std::cout << "ERROR: Invalid input arguement(s)." << std::endl;
      std::cout << "Correct usage: " << argv[0] << " <TISSUE_TYPE> <MORPHOLOGICAL_OPERATOR> <DICOM_Folder_Name/Filename.nii>" << std::endl;
      return EXIT_FAILURE;
  }

  switch( inputFiletype.type )
  {
      case 0:     // Directory with DICOM series
      {
          // Read all files from the DICOM series in the specified directory.
          DICOM_reader = vtkSmartPointer<vtkDICOMImageReader>::New();
          DICOM_reader->SetDirectoryName( inputFile.c_str() );
          DICOM_reader->Update();

          volume->DeepCopy( DICOM_reader->GetOutput() );

          break;
      }

      case 1:     // NIfTI
      {
          // Create a reader and check if the input file is readable.
          NIFTI_reader = vtkSmartPointer<vtkNIFTIImageReader>::New();

          if ( !( NIFTI_reader->CanReadFile( inputFile.c_str() ) ) )
          {
              std::cout << "ERROR: vtk NIFTI image reader cannot read the provided file: " << inputFile << std::endl;
              return EXIT_FAILURE;
          }

          NIFTI_reader->SetFileName( inputFile.c_str() );
          NIFTI_reader->Update();

          volume->DeepCopy( NIFTI_reader->GetOutput() );

          break;
      }

      default:
      {
          return EXIT_FAILURE;
      }
  } 

//   using PixelType = unsigned char;
//   using ImageType = itk::Image< PixelType, 3 >;

//     using FilterType = itk::VTKImageToImageFilter< ImageType >;
//     FilterType::Pointer filter = FilterType::New();
//     filter->SetInput( volume );

//     using otsu = itk::OtsuMultipleThresholdsImageFilter< ImageType, ImageType >;
//   otsu::Pointer otsu_filter = FilterType::New();
//   otsu_filter->SetInput( filter->GetOutput() );
//   otsu_filter->SetNumberOfHistogramBins( 10 );
//   otsu_filter->SetNumberOfThresholds( 1 );
//   otsu_filter->SetLabelOffset( 1 );

//   using temp = itk::ImageToVTKImageFilter< ImageType >;
//   temp::Pointer t = FilterType::New();
//   t->SetInput( otsu_filter->GetOutput() );
//   vtkImageData * myvtkImageData = t->GetOutput();

   // Create the renderer and render window
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground( 0, 0, 0 );

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer( renderer );
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow( renderWindow );

        std::cout << "\nStarting surface rendering...\n";

        // Perform segmentation to extract bone
        int lowerThresh = 0, upperThresh = 0;
        double isoValue = 0.0;

        // Get the threshold and isovalue parameters from the user
        std::cout << "Performing image segmentation \n";
        std::cout << "Please enter upper and lower threshold values: \n";
        std::cout << "Lower Threshold = ";
        std::cin >> lowerThresh;
        std::cout << "Upper Threshold = ";
        std::cin >> upperThresh;

        std::cout << "Please enter the desired isovalue for the Marching Cubes algortihm: ";
        std::cin >> isoValue;

        // Apply the global threshold
        vtkSmartPointer<vtkImageThreshold> globalThresh = vtkSmartPointer<vtkImageThreshold>::New();
        globalThresh->SetInputData( volume );
        globalThresh->ThresholdBetween( lowerThresh, upperThresh );
        globalThresh->ReplaceInOn();
        globalThresh->SetInValue( isoValue + 1 );
        globalThresh->ReplaceOutOn();
        globalThresh->SetOutValue(0);
        globalThresh->SetOutputScalarTypeToFloat();
        globalThresh->Update();

        volume = globalThresh->GetOutput();

        // Use the Marching cubes algorithm to generate the surface
        std::cout << "Generating surface using Marching cubes \n";

        vtkSmartPointer<vtkMarchingCubes> surface = vtkSmartPointer<vtkMarchingCubes>::New();
        surface->SetInputData( volume );
        surface->ComputeNormalsOn();
        surface->SetValue( 0, isoValue );

        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection( surface->GetOutputPort() );
        mapper->ScalarVisibilityOff();

        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper( mapper );

        renderer->AddActor( actor );

        renderWindow->Render();
        interactor->Start();

    return EXIT_SUCCESS;
}