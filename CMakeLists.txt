cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(lightning-search)

# Find the LibTorch package
#find_package(Torch REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# Set C++ standard to 14 or 17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(ls 
src/main.cpp 
src/WebServer.cpp 
src/TcpListener.cpp) # source files

target_include_directories(ls PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src) # add header



# Link LibTorch libraries
#target_link_libraries(ls "${TORCH_LIBRARIES}")

# Required for some Linux systems to avoid ABI issues
set_property(TARGET ls PROPERTY CXX_STANDARD 20)