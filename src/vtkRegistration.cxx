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
    // Check the number of input parameters
    if ( argc != 3 )
    {
        std::cout << "ERROR: Incorrect program usage." << std:: endl;
        std::cout << "Correct usage: " << argv[0] << " <DICOM_Folder_Directory> <OBJ_File_Directory>" << std::endl;
        return EXIT_FAILURE;
    }

    vtkSmartPointer<vtkImageData> volume = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkPolyData>  obj    = vtkSmartPointer<vtkPolyData>::New();

    vtkSmartPointer<vtkDICOMImageReader> dicomReader;
    vtkSmartPointer<vtkOBJReader>        objReader;

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

    volume->DeepCopy( dicomReader->GetOutput() );

    // Read in the OBJ file
    objReader = vtkSmartPointer<vtkOBJReader>::New();
    objReader->SetFileName( objFile.c_str() );
    objReader->Update();

    obj->DeepCopy( objReader->GetOutput() );

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
    globalThresh->SetOutValue( 0 );
    globalThresh->SetOutputScalarTypeToFloat();
    globalThresh->Update();

    volume = globalThresh->GetOutput();

    // Use the Marching cubes algorithm to generate the surface
    std::cout << "Generating surface using Marching cubes \n";

    vtkSmartPointer<vtkMarchingCubes> surface = vtkSmartPointer<vtkMarchingCubes>::New();
    surface->SetInputData( volume );
    surface->ComputeNormalsOn();
    surface->SetValue( 0, 1 );

    // Perform ICP registration
    vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
    icp->SetSource( surface->GetOutput() );
    icp->SetTarget( obj );
    icp->SetMaximumNumberOfIterations( 20 );
    icp->GetLandmarkTransform()->SetModeToRigidBody();
    icp->StartByMatchingCentroidsOn();
    icp->Modified();
    icp->Update();

    vtkSmartPointer<vtkMatrix4x4> m = icp->GetMatrix();
    std::cout << "The resulting matrix is: " << *m << std::endl;

    vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    icpTransformFilter->SetInputData( surface->GetOutput() );
    icpTransformFilter->SetTransform( icp );
    icpTransformFilter->Update();

    // Image reslicing
    vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
    reslice->SetInputConnection( surface->GetOutputPort() );
    reslice->InterpolateOn();
    reslice->SetResliceTransform( m );
    reslice->Upate();

    // Define mappers
    vtkSmartPointer<vtkPolyDataMapper> sourceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    sourceMapper->SetInputData( reslice->GetOutput() );

    vtkSmartPointer<vtkActor> sourceActor = vtkSmartPointer<vtkActor>::New();
    sourceActor->SetMapper( sourceMapper );
    sourceActor->GetProperty()->SetColor( 1, 0, 0 );
    sourceActor->GetProperty()->SetPointSize( 4 );

    vtkSmartPointer<vtkPolyDataMapper> targetMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

    targetMapper->SetInputData( obj );

    vtkSmartPointer<vtkActor> targetActor = vtkSmartPointer<vtkActor>::New();
    targetActor->SetMapper( targetMapper );
    targetActor->GetProperty()->SetColor( 0, 1, 0);
    targetActor->GetProperty()->SetPointSize( 4 );
  
    vtkSmartPointer<vtkPolyDataMapper> solutionMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    solutionMapper->SetInputConnection( icpTransformFilter->GetOutputPort() );

    vtkSmartPointer<vtkActor> solutionActor = vtkSmartPointer<vtkActor>::New();
    solutionActor->SetMapper( solutionMapper );
    solutionActor->GetProperty()->SetColor( 0, 0, 1 );
    solutionActor->GetProperty()->SetPointSize( 3 );

    renderer->AddActor( sourceActor );
    renderer->AddActor( targetActor );
    renderer->AddActor( solutionActor );
    renderer->SetBackground( 0.3, 0.2, 0.6 );

    // vtkSmartPointer<vtkPolyDataMapper> surfaceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    // surfaceMapper->SetInputConnection( surface->GetOutputPort() );
    // surfaceMapper->ScalarVisibilityOff();

    // vtkSmartPointer<vtkPolyDataMapper> objMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    // objMapper->SetInputConnection( obj );

    // Create actors
    // vtkSmartPointer<vtkActor> objActor = vtkSmartPointer<vtkActor>::New();
    // objActor->SetMapper( objMapper );

    // vtkSmartPointer<vtkActor> dicomActor = vtkSmartPointer<vtkActor>::New();
    // dicomActor->SetMapper( surfaceMapper );

    // Render the scene
    // renderer->AddActor( objActor );
    // renderer->AddActor( dicomActor );

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}