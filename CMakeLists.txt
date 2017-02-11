cmake_minimum_required(VERSION 3.0)
project(TinyMoeFace_Research1_ARAP)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package(CGAL REQUIRED)
include( ${CGAL_USE_FILE} )
include_directories( ${CGAL_INCLUDE_DIRS} )

include_directories(
  ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS})

add_definitions(${Qt5Widgets_DEFINITIONS})

find_package(Qt5Widgets REQUIRED)

add_library(DeformDialog.ui DeformDialog.ui)

add_executable(${PROJECT_NAME} main.cpp DeformDialog.cpp DeformWidget.cpp FaceOutlineReader.cpp CGALTriangulator.cpp)
qt5_use_modules(${PROJECT_NAME} Widgets Gui Core)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} ${CGAL_LIBS})
