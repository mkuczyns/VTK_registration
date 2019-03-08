# VTK_Registration

Performs Iterative Closest Point Image Registration on a DICOM series and a surface (.OBJ) file of a spine.

## Program Steps
1. Read in DICOM dataset and OBJ file
2. Filter the DICOM series
3. Apply a global threshold to the DICOM series
4. Use the Marching Cubes algorithm to generate a surface rendering of the DICOM series
5. Perform ICP registration and save the transformation matrix
6. Transform the original DICOM image to the OBJ image space
7. Re-apply the threshold and surface rendering steps on the transformed DICOM image
8. Visualize the results (overlay)

## How to Run
1. Create a folder for the build (e.g. bin, build, etc.)
2. Build with CMake and your favorite compiler.
3. Run the executable that is generated in the bin\Debug folder from the command line
   
    ```
    vtkRegistration.exe <PATH_TO_DICOM_FOLDER> <PATH_TO_OBJ_FILE>
    ```
4. Follow the prompts after running the above command to enter required threshold and isosurface values

## Notes
- The amount of triangles used in the Marching Cubes algorithm is reduced to half to decrease computation time
- The user can enter the threshold limits, however, for the spine image provided in this assignment it is recommended to use values of -800 and -600
- The maximum number of iterations performed by the registration is set to 75, but can be increased if needed
- The original (untransformed) DICOM image can be visualized along with the registered result, but has been commented out to decrease computation time