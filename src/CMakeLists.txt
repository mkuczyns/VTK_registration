cmake_minimum_required(VERSION 2.8)

PROJECT(vtkRegistration)

find_package(VTK REQUIRED)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
else()
  find_package(ItkVtkGlue REQUIRED)
  include(${ItkVtkGlue_USE_FILE})
  set(Glue ItkVtkGlue)
endif()

add_executable(vtkRegistration MACOSX_BUNDLE vtkRegistration.cxx helperFunctions.cxx interactorStyler.cxx)

if(VTK_LIBRARIES)
  target_link_libraries(vtkRegistration ${VTK_LIBRARIES})
else()
  target_link_libraries(vtkRegistration vtkHybrid vtkWidgets)
endif()

target_link_libraries(vtkRegistration ${ITK_LIBRARIES})
