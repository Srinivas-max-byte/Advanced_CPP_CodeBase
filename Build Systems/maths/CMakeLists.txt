# Specify the minimum version of CMake required to build this project
# This ensures compatibility and prevents using older CMake versions that might not support required features
cmake_minimum_required(VERSION 3.10)

# Define the project name for this library
# This sets variables like PROJECT_NAME, PROJECT_SOURCE_DIR, etc.
project(maths)

# Set the C++ standard version to use for compilation
# C++11 provides features like auto, nullptr, lambda functions, etc.
set(CMAKE_CXX_STANDARD 11)

# Make the C++ standard requirement mandatory
# If the compiler doesn't support C++11, CMake will generate an error instead of just a warning
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Traditional/legacy way to add include directories globally
# This adds the 'include' directory to the compiler's include search path for all targets
# Note: This affects all targets in this CMakeLists.txt, but not targets that link to this library
include_directories(include)

# Define a variable containing all source files needed for the library
# Using a variable makes it easier to manage and maintain the list of sources
set(SOURCES
    src/math.cpp    # Implementation file containing math utility functions
)

# Create a static library target named 'maths'
# STATIC means the library will be compiled into a .lib (Windows) or .a (Linux) file
# The library code is copied into any executable that links to it
# This must be called BEFORE using target-specific commands like target_include_directories
add_library(maths STATIC ${SOURCES})

# Modern/target-based way to specify include directories
# PUBLIC means: include directories are used both when building this library AND when other targets link to it
# Note: Using namespaces in library source code is essential because it prevents naming conflicts when multiple
#       libraries are linked together. Without namespaces, functions like "square()" from different libraries
#       would collide, causing compilation errors. Namespaces provide a scope that isolates library symbols
#       and allows multiple libraries to coexist safely in the same project (e.g., math_function::square())
# PRIVATE means: include directories are used only when building this library itself, but NOT propagated to targets that link to it
# INTERFACE means: include directories are NOT used when building this library, but ARE propagated to targets that link to it (header-only libraries)
# This is more efficient than include_directories because it automatically propagates to dependent targets
# IMPORTANT: This must come AFTER add_library, because you can only set properties on existing targets
target_include_directories(maths PUBLIC include)

