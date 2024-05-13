# CMakeLists.txt
cmake_minimum_required(VERSION 3.0)

enable_language(C)

# Set the C compiler
# Replace "path/to/your/gcc.exe" with the actual path to your GCC compiler executable

set(CMAKE_C_COMPILER "C:/MinGW/bin/gcc.exe")  
set(CMAKE_CXX_COMPILER "C:/MinGW/bin/g++.exe")  # Uncomment and replace "path/to/your/g++.exe" if you are using C++

# Add your source files here
set(SOURCE_FILES
    main.c
    Func_dataIO.c
    Func_Initialize.c
    Func_SSIM.c
    Func_Disaggregate.c
    Func_kNN.c
    Func_fragments.c
)

project(kNN_MOF_SSIM)  # Set your project name here
# Add the executable target
add_executable(kNN_MOF_SSIM ${SOURCE_FILES})


## cmake -G "MinGW Makefiles" .
## mingw32-make
