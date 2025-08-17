cmake_minimum_required(VERSION 3.14)

project(ipsec--status-parser LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_executable(ipsec--status-parser
    parser.cpp
    parser.h
    reader.cpp
    reader.h
  main.cpp
)

target_link_libraries( ${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core)

install( TARGETS ${PROJECT_NAME} )
