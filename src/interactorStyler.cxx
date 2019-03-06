/****************************************************************************
*   interactorStyler.cxx
*
*   Created by:     Michael Kuczynski
*   Created on:     19/01/2019
*   Description:    Custom interactor styling implementation.
****************************************************************************/

#include "interactorStyler.hxx"

void myInteractorStyler::setImageViewer( vtkImageViewer2* imageViewer )
{
    _ImageViewer = imageViewer;
    minSlice = imageViewer->GetSliceMin();
    maxSlice = imageViewer->GetSliceMax();
    windowLevel = imageViewer->GetWindowLevel()->GetLevel();
    window = imageViewer->GetWindowLevel()->GetWindow();
    
    // Start current slice at 0
    slice = minSlice;
}

void myInteractorStyler::setSliceStatusMapper( vtkTextMapper* statusMapper ) 
{
    _SliceStatusMapper = statusMapper;
}

void myInteractorStyler::setWindowLevelStatusMapper( vtkTextMapper* statusMapper ) 
{
    _WindowLevelStatusMapper = statusMapper;
}

void myInteractorStyler::setWindowStatusMapper( vtkTextMapper* statusMapper ) 
{
    _WindowStatusMapper = statusMapper;
}

void myInteractorStyler::moveSliceForward() 
{
    if ( slice < maxSlice ) 
    {
        slice += 1;

        _ImageViewer->SetSlice( slice );

        // Create the message to be displayed.
        std::string msg = ImageMessage::sliceNumberFormat( slice, maxSlice );

        // Update the mapper and render.
        _SliceStatusMapper->SetInput( msg.c_str() );
        _ImageViewer->Render();
    }
}

void myInteractorStyler::moveSliceBackward() 
{
    if ( slice > minSlice ) 
    {
        slice -= 1;

        _ImageViewer->SetSlice( slice );

        // Create the message to be displayed.
        std::string msg = ImageMessage::sliceNumberFormat( slice, maxSlice );

        // Update the mapper and render.
        _SliceStatusMapper->SetInput( msg.c_str() );
        _ImageViewer->Render();
    }
}

void myInteractorStyler::moveWindowLevelForward() 
{
    windowLevel += 5;

    _ImageViewer->GetWindowLevel()->SetLevel( windowLevel );

    // Create the message to be displayed.
    std::string msg = ImageMessage::windowLevelFormat( int( windowLevel ) );

    // Update the mapper and render.
    _WindowLevelStatusMapper->SetInput( msg.c_str() );
    _ImageViewer->Render();
}

void myInteractorStyler::moveWindowLevelBackward() 
{
    windowLevel -= 5;

    _ImageViewer->GetWindowLevel()->SetLevel( windowLevel );

    // Create the message to be displayed.
    std::string msg = ImageMessage::windowLevelFormat( int( windowLevel ) );

    // Update the mapper and render.
    _WindowLevelStatusMapper->SetInput( msg.c_str() );
    _ImageViewer->Render();
}

void myInteractorStyler::moveWindowForward()
{
    window += 5;

    _ImageViewer->GetWindowLevel()->SetWindow( window );

    // Create the message to be displayed.
    std::string msg = ImageMessage::windowFormat( int( window ) );

    // Update the mapper and render.
    _WindowStatusMapper->SetInput( msg.c_str() );
    _ImageViewer->Render();
}

void myInteractorStyler::moveWindowBackward()
{
    window -= 5;

    _ImageViewer->GetWindowLevel()->SetWindow( window );

    // Create the message to be displayed.
    std::string msg = ImageMessage::windowFormat( int( window ) );

    // Update the mapper and render.
    _WindowStatusMapper->SetInput( msg.c_str() );
    _ImageViewer->Render();   
}