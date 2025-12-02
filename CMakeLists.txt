cmake_minimum_required(VERSION 3.20)
project(bilsimulator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# threepp
FetchContent_Declare(
        threepp
        GIT_REPOSITORY https://github.com/markaren/threepp.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(threepp)

# Catch2 (v3)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.7.0
)
FetchContent_MakeAvailable(Catch2)

# hovedprogram
add_executable(car
        src/main.cpp
        src/models/Car.cpp
        src/models/CameraRig.cpp
        src/world/Parking.cpp
        src/world/TrafficCones.cpp
        src/logic/Game.cpp
)

target_include_directories(car PRIVATE include)
target_link_libraries(car PRIVATE threepp)

# legg exe (og .dll) i bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# --- tester ---

enable_testing()

add_executable(car_tests
        tests/test_car.cpp
        tests/test_parking.cpp
)

target_include_directories(car_tests PRIVATE include)
target_link_libraries(car_tests PRIVATE threepp Catch2::Catch2WithMain)

add_test(NAME car_tests COMMAND car_tests)

