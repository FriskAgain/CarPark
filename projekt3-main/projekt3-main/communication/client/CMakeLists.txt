cmake_minimum_required(VERSION 3.10) 

project(client LANGUAGES CXX) 

# Set C++ standard 
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF) 

# Add include path 
include_directories(${CMAKE_SOURCE_DIR}/external/json_dto)
# Create executable from source files 
add_executable(client.exe
    src/client.cpp 
    src/I2Cdriver.cpp 
    src/Display.cpp 
    src/Button.cpp) 
    
# If I2Cdriver needs external libraries (e.g., -lrt), link them here: 
find_package(CURL REQUIRED)
target_link_libraries(client.exe PRIVATE CURL::libcurl)
target_link_libraries(client.exe PRIVATE SSD1306_OLED_RPI)