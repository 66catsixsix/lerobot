cmake_minimum_required(VERSION 3.14)

project(LeRobotFeetechBase LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
)


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

add_executable(robot_gui
    qml/main.cpp
    qml/robot_backend.hpp
    qml/robot_backend.cpp
    base/serial.cpp
    base/feetech.cpp
)

target_include_directories(robot_gui PRIVATE
    base
    SCServo_Linux/include
)

target_link_libraries(robot_gui PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    SCServo
)

target_compile_definitions(robot_gui PRIVATE
    SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)
