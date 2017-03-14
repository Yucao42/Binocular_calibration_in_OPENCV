cmake_minimum_required(VERSION 2.8)
project(Zed_calibration)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

# Check C++11 or C++0x support
add_definitions(-DCOMPILEDWITHC11)
if( CMAKE_BUILD_TYPE MATCHES "Debug" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -msse3 -std=c++11 -pthread -g -march=native")
else( CMAKE_BUILD_TYPE MATCHES "Release" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -msse3 -std=c++11 -pthread -O3 -march=native")
endif( CMAKE_BUILD_TYPE MATCHES "Debug" )

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

find_package(OpenCV 3.1.0 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(ZED  REQUIRED)
find_package(CUDA  REQUIRED)

include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
${EIGEN3_INCLUDE_DIR}
${CUDA_INCLUDE_DIRS}
${ZED_INCLUDE_DIRS}
${OpenCV_INCLUDE_DIRS}
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} SHARED
src/binocular_calibration.cpp
src/zed_read.cpp
)

target_link_libraries(${PROJECT_NAME}
${OpenCV_LIBS}
${ZED_LIBRARIES}######Easy to forget
${EIGEN3_LIBS}
)

link_directories(${ZED_LIBRARY_DIR})
link_directories(${OpenCV_LIBRARY_DIRS})
link_directories(${CUDA_LIBRARY_DIRS})

add_executable(calibration_main Example/calibration_main.cpp)
target_link_libraries(calibration_main  
                        ${ZED_LIBRARIES}
                        ${OpenCV_LIBRARIES}
                        ${CUDA_LIBRARIES} ${CUDA_nppi_LIBRARY} ${CUDA_npps_LIBRARY}
${EIGEN3_LIBS}
                        ${PROJECT_NAME}
                    )
