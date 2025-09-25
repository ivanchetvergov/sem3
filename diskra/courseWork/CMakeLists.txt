cmake_minimum_required(VERSION 3.10)
project(FiniteCalculator VERSION 1.0)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(core/include)

find_package(yaml-cpp REQUIRED)

add_executable(test_runner
    core/src/FiniteField.cc
    core/src/FiniteFieldRules.cc
    core/test.cc
)

target_include_directories(test_runner PUBLIC ${YAML_CPP_INCLUDE_DIR})
target_link_directories(test_runner PUBLIC ${YAML_CPP_LIBRARY_DIR})

target_link_libraries(test_runner PRIVATE yaml-cpp)

add_test(NAME Test_Z8_Variant_1 COMMAND test_runner variant_1)
add_test(NAME Test_Z8_Variant_2 COMMAND test_runner variant_2)
# add_test(NAME Test_Z8_X1 COMMAND test_runner X1)
add_test(NAME Test_Z11_D1 COMMAND test_runner D1)