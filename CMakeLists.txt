cmake_minimum_required(VERSION 3.15)
project(csopesy CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

add_executable(csopesy
    src/main.cpp
    src/Console.cpp
    src/Config.cpp
    src/Scheduler.cpp
    src/Process.cpp
    src/Instruction.cpp
    src/ProgramGenerator.cpp
    src/Utils.cpp
)
target_link_libraries(csopesy PRIVATE Threads::Threads)
