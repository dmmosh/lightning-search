cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(lightning-search)

# Find the LibTorch package
#find_package(Torch REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# Set C++ standard to 14 or 17
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
GIT_TAG c492b4fed1fd7288eacec6fe8d89e37df78d373a) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr) # include cpr from repo

find_package(ZLIB REQUIRED) # include zlib 

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_executable(ls 
src/main.cpp 
src/WebServer.cpp 
src/TcpListener.cpp) # source files

set_property(TARGET ls PROPERTY CXX_STANDARD 20)
target_include_directories(ls PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src) # add header
target_link_libraries(ls PRIVATE cpr::cpr ZLIB::ZLIB)



# Link LibTorch libraries
#target_link_libraries(ls "${TORCH_LIBRARIES}")

# Required for some Linux systems to avoid ABI issues