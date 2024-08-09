# CMakeLists.txt
cmake_minimum_required(VERSION 3.0)

project(kNN_MOF_SSIM)  # Set your project name here
enable_language(C)

# Add your source files here
set(SOURCE_FILES
    main.c
    Func_dataIO.c
    Func_Initialize.c
    Func_SSIM.c
    Func_ASSIM.c
    Func_wSSIM.c
    Func_Disaggregate.c
    Func_kNN.c
    Func_Fragments.c
    Func_Prepro.c
    Func_Recursive.c
    Func_Print.c
)

# Add the executable target
add_executable(kNN_MOF_SSIM ${SOURCE_FILES})

# Link against the math library
target_link_libraries(kNN_MOF_SSIM m)

## cmake -G "MinGW Makefiles" .
## mingw32-make
