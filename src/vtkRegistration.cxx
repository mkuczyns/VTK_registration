/****************************************************************************
*   vtkRegistration.cxx
*
*   Created by:     Michael Kuczynski
*   Created on:     05/03/2019
*   Version:        1.0
*   Description:    
****************************************************************************/

#include "interactorStyler.hxx"

vtkStandardNewMacro(myInteractorStyler);

int main(int argc, char* argv[])
{
    /***************************************************************
    *   Check input arguements
    ***************************************************************/
    if ( argc != 3 )
    {
        std::cout << "ERROR: Incorrect program usage." << std:: endl;
        std::cout << "Correct usage: " << argv[0] << " <DICOM_Folder_Directory> <OBJ_File_Directory>" << std::endl;
        return EXIT_FAILURE;
    }

    vtkSmartPointer<vtkPolyData> obj = vtkSmartPointer<vtkPolyData>::New();

    vtkSmartPointer<vtkDICOMImageReader> dicomReader;
    vtkSmartPointer<vtkOBJReader>        objReader;

    /***************************************************************
    *   Read in the provided DICOM and OBJ files
    ***************************************************************/
    // Create a variable for the input arguement (if valid type).
    std::string dicomFile = "";
    std::string objFile = "";

    // Verify that the provided input arguements are valid
    if ( !checkInputs( argv[1], argv[2] ) )
    {
        return EXIT_FAILURE;
    }
    else
    {
        dicomFile = argv[1];
        objFile = argv[2];
    }

    // Read all files from the DICOM series in the specified directory.
    dicomReader = vtkSmartPointer<vtkDICOMImageReader>::New();
    dicomReader->SetDirectoryName( dicomFile.c_str() );
    dicomReader->Update();

    // Read in the OBJ file
    objReader = vtkSmartPointer<vtkOBJReader>::New();
    objReader->SetFileName( objFile.c_str() );
    objReader->Update();

    obj = objReader->GetOutput();

    /***************************************************************
    *   Apply a Gaussian filter to the DICOM series
    ***************************************************************/
    vtkSmartPointer<vtkImageGaussianSmooth> gaussianSmoothFilter = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    gaussianSmoothFilter->SetInputConnection( dicomReader->GetOutputPort() );
    gaussianSmoothFilter->SetStandardDeviation( 1.0 );
    gaussianSmoothFilter->SetRadiusFactors( 1.0, 1.0, 1.0 );
    gaussianSmoothFilter->SetDimensionality( 3 );
    gaussianSmoothFilter->Update();

    /***************************************************************
    *   Segment the input DICOM series
    ***************************************************************/
    int lowerThresh = 0, upperThresh = 0;
    double isoValue = 0.0;

    // Get the threshold and isovalue parameters from the user
    std::cout << "\n**Performing image segmentation** \n";
    std::cout << "Please enter upper and lower threshold values (for this assignment, it is recommended to select values between -800 to -600): \n";
    std::cout << "Lower Threshold = ";
    std::cin >> lowerThresh;
    std::cout << "Upper Threshold = ";
    std::cin >> upperThresh;

    std::cout << "Applying global threshold...";

    // Apply the global threshold
    vtkSmartPointer<vtkImageThreshold> globalThresh = vtkSmartPointer<vtkImageThreshold>::New();
    globalThresh->SetInputData( gaussianSmoothFilter->GetOutput() );
    globalThresh->ThresholdBetween( lowerThresh, upperThresh );
    globalThresh->ReplaceInOn();
    globalThresh->SetInValue( 1 );
    globalThresh->ReplaceOutOn();
    globalThresh->SetOutValue( 0 );
    globalThresh->SetOutputScalarTypeToFloat();
    globalThresh->Update();

    std::cout << "Done! \n";

    /***************************************************************
    *   Use the Marching cubes algorithm to generate the surface
    ***************************************************************/
    std::cout << "\n**Generating surface using Marching cubes** \n";

    std::cout << "Please enter the desired isovalue for the Marching Cubes algortihm (between 0-1): ";
    std::cin >> isoValue;

    std::cout << "Starting surface rendering...";

    vtkSmartPointer<vtkMarchingCubes> surface = vtkSmartPointer<vtkMarchingCubes>::New();
    surface->SetInputData( globalThresh->GetOutput() );
    surface->ComputeNormalsOn();
    surface->SetValue( 0, isoValue );

    // Reduce the number of triangles by half to speed up computation
    vtkSmartPointer<vtkDecimatePro> decimate = vtkSmartPointer<vtkDecimatePro>::New();
    decimate->SetInputConnection( surface->GetOutputPort() );
    decimate->SetTargetReduction( 0.5 );
    decimate->Update();

    std::cout << "Done! \n";

    /***************************************************************
    *   Perform ICP registration
    ***************************************************************/
    std::cout << "\n**Starting image registration** \n";

    vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
    icp->SetSource( obj );
    icp->SetTarget( decimate->GetOutput() );
    icp->SetMaximumNumberOfIterations( 100 );
    icp->GetLandmarkTransform()->SetModeToRigidBody();
    icp->StartByMatchingCentroidsOn();
    icp->Update();

    vtkSmartPointer<vtkMatrix4x4> m = icp->GetMatrix();
    std::cout << "\nThe resulting transformation matrix is: \n" << std::fixed << std::setprecision(2) << *m;

    vtkSmartPointer<vtkTransform> icpTransformFilter = vtkSmartPointer<vtkTransform>::New();
    icpTransformFilter->SetMatrix( icp->GetMatrix() );
    icpTransformFilter->Update();

    // Image reslicing of the filtered image

    std::cout << "Transforming the original image into the new coordinate space...";

    vtkSmartPointer<vtkImageData> gaussianSmoothFilterTransform = vtkSmartPointer<vtkImageData>::New();
    gaussianSmoothFilterTransform->DeepCopy( gaussianSmoothFilter->GetOutput() );

    vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
    reslice->SetInputData( gaussianSmoothFilterTransform );
    reslice->InterpolateOff();  // On for nearest neighbour, off for linear
    reslice->AutoCropOutputOn();
    reslice->SetResliceTransform( icpTransformFilter );
    reslice->Update();

    std::cout << "Done! \n";

    std::cout << "Performing image segmentation on the transformed image...";

    // Now render the transformed image
    // Since we had to reslice the original image, we will need to segment and render the resliced image again...
    vtkSmartPointer<vtkImageThreshold> globalThreshTransformed = vtkSmartPointer<vtkImageThreshold>::New();
    globalThreshTransformed->SetInputData( reslice->GetOutput() );
    globalThreshTransformed->ThresholdBetween( lowerThresh, upperThresh );
    globalThreshTransformed->ReplaceInOn();
    globalThreshTransformed->SetInValue( 1 );
    globalThreshTransformed->ReplaceOutOn();
    globalThreshTransformed->SetOutValue( 0 );
    globalThreshTransformed->SetOutputScalarTypeToFloat();
    globalThreshTransformed->Update();

    std::cout << "Done! \n";

    std::cout << "Starting surface rendering...";

    vtkSmartPointer<vtkMarchingCubes> surfaceTransformed = vtkSmartPointer<vtkMarchingCubes>::New();
    surfaceTransformed->SetInputData( globalThreshTransformed->GetOutput() );
    surfaceTransformed->ComputeNormalsOn();
    surfaceTransformed->SetValue( 0, isoValue );

    // Reduce the number of triangles by half to speed up computation
    vtkSmartPointer<vtkDecimatePro> decimateTransformed = vtkSmartPointer<vtkDecimatePro>::New();
    decimateTransformed->SetInputConnection( surfaceTransformed->GetOutputPort() );
    decimateTransformed->SetTargetReduction( 0.5 );
    decimateTransformed->Update();

    std::cout << "Done! \n";

    /***************************************************************
    *   Add mappers, actors, renderer, and setup the scene
    ***************************************************************/
    std::cout << "\n**Rendering the scene** \n";

    // Define mappers
    // 1. Original Image
    // vtkSmartPointer<vtkPolyDataMapper> sourceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    // sourceMapper->SetInputData( decimate->GetOutput() );

    // vtkSmartPointer<vtkActor> sourceActor = vtkSmartPointer<vtkActor>::New();
    // sourceActor->SetMapper( sourceMapper );
    // sourceActor->GetProperty()->SetColor( 1, 0, 0 );
    // sourceActor->GetProperty()->SetPointSize( 4 );
    // sourceActor->GetMapper()->ScalarVisibilityOff();

    // 2. OBJ File
    vtkSmartPointer<vtkPolyDataMapper> targetMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    targetMapper->SetInputData( obj );

    vtkSmartPointer<vtkActor> targetActor = vtkSmartPointer<vtkActor>::New();
    targetActor->SetMapper( targetMapper );
    targetActor->GetProperty()->SetColor( 0, 1, 0 );
    targetActor->GetProperty()->SetOpacity( 0.5 );
    targetActor->GetProperty()->SetPointSize( 4 );
    targetActor->GetMapper()->ScalarVisibilityOff();
  
    // 3. Registered Image
    vtkSmartPointer<vtkPolyDataMapper> solutionMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    solutionMapper->SetInputConnection( decimateTransformed->GetOutputPort() );

    vtkSmartPointer<vtkActor> solutionActor = vtkSmartPointer<vtkActor>::New();
    solutionActor->SetMapper( solutionMapper );
    solutionActor->GetProperty()->SetColor( 0, 0, 1 );
    solutionActor->GetProperty()->SetPointSize( 4 );
    solutionActor->GetMapper()->ScalarVisibilityOff();

    // Create the renderer and render window
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer( renderer );
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow( renderWindow );

    // renderer->AddActor( sourceActor );
    renderer->AddActor( targetActor );
    renderer->AddActor( solutionActor );
    renderer->SetBackground( 0.0, 0.0, 0.0 );

    renderWindow->Render();

    std::cout << "Done! \n";

    interactor->Start();
    
    return EXIT_SUCCESS;
}