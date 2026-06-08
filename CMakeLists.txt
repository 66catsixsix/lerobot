cmake_minimum_required(VERSION 3.14)

project(LeRobotFeetechBase LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(SCServo_Linux)

add_executable(robot_base
    base/base.cpp
    base/serial.cpp
    base/feetech.cpp
)

target_link_libraries(robot_base PRIVATE
    SCServo
)

target_include_directories(robot_base PRIVATE
    ./SCServo_Linux/include/
)
