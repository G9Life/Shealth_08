cmake_minimum_required(VERSION 3.10)
project(CoreProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main library
add_library(shealth_lib
    src/main/cpp/AgeDecadePolicy.cpp
    src/main/cpp/BmiCalculator.cpp
    src/main/cpp/BmiDistributionAnalyzer.cpp
    src/main/cpp/HealthRecordCsvReader.cpp
    src/main/cpp/SHealth.cpp
    src/main/cpp/WeightImputer.cpp
)
target_include_directories(shealth_lib PUBLIC src/main/cpp)

# Main executable
add_executable(SHealthBMI src/main/cpp/SHealthBMI.cpp)
target_link_libraries(SHealthBMI PRIVATE shealth_lib)
set_target_properties(SHealthBMI PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Google Test (reuse cached source when available)
set(GOOGLETEST_SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/googletest-src")
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
if(EXISTS "${GOOGLETEST_SOURCE_DIR}/CMakeLists.txt")
    add_subdirectory("${GOOGLETEST_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/googletest-build" EXCLUDE_FROM_ALL)
else()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()

# Test executable
add_executable(SHealthBMITest src/test/cpp/SHealthBMITest.cpp)
target_link_libraries(SHealthBMITest PRIVATE shealth_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(SHealthBMITest WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
