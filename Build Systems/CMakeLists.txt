# Specify the minimum version of CMake required to build this project
# This ensures compatibility and prevents using older CMake versions that might not support required features
cmake_minimum_required(VERSION 3.10)

# Define the project name and optionally the programming languages used
# This sets variables like PROJECT_NAME, PROJECT_SOURCE_DIR, etc.
project(CalculatorApp)

# Set the C++ standard version to use for compilation
# C++11 provides features like auto, nullptr, lambda functions, etc.
set(CMAKE_CXX_STANDARD 11)

# Make the C++ standard requirement mandatory
# If the compiler doesn't support C++11, CMake will generate an error instead of just a warning
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories to the compiler's include search path
# This allows source files to find header files using #include "Calculator.h" instead of full paths
# Note: This is a global setting affecting all targets in this CMakeLists.txt
include_directories(Calculator_app/include)

# Define a variable containing all source files needed for the executable
# Using a variable makes it easier to manage and maintain the list of sources
set(SOURCES
    src/main.cpp                              # Main entry point of the application
    Calculator_app/src/Calculator.cpp         # Calculator class implementation
    maths/src/math.cpp                        # Math utility functions implementation
)

# Process the CMakeLists.txt file in the 'maths' subdirectory
# This builds the 'maths' library target defined in that subdirectory
# Must be called before targets in this file try to link to 'maths'
add_subdirectory(maths)

# Process the CMakeLists.txt file in the 'physics' subdirectory
# This builds the 'physics' shared library target defined in that subdirectory
# Must be called before targets in this file try to link to 'physics'
add_subdirectory(physics)

# Create an executable target named 'calculator_app'
# The executable will be built from all source files listed in ${SOURCES}
# ${SOURCES} expands to the list of source files defined above
add_executable(calculator_app ${SOURCES})

# Link the 'maths' library to the 'calculator_app' executable
# This makes the functions and classes from the 'maths' library available to the executable
# The library must be built before linking (handled by add_subdirectory above)
target_link_libraries(calculator_app maths)

# Link the 'physics' shared library to the 'calculator_app' executable
# This makes the functions and classes from the 'physics' library available to the executable
# Since physics is a SHARED library, the .dll/.so file must be present at runtime
# The library must be built before linking (handled by add_subdirectory above)
target_link_libraries(calculator_app physics) #Can be done in same line as above.

