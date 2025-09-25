cmake_minimum_required(VERSION 3.10)
project(FiniteCalculator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(core/include)

find_package(pybind11 REQUIRED)
find_package(yaml-cpp REQUIRED)

# модуль для python
pybind11_add_module(finite_field_module
    core/src/FiniteField.cc
    core/src/FiniteFieldRules.cc
    core/py_binding.cc
)
target_link_libraries(finite_field_module PRIVATE yaml-cpp::yaml-cpp)


add_executable(test_runner
    core/src/FiniteField.cc
    core/src/FiniteFieldRules.cc
    core/test.cc
)
target_link_libraries(test_runner PRIVATE yaml-cpp::yaml-cpp)

enable_testing()
add_test(NAME Test_Z8_Variant_1 COMMAND test_runner variant_1)
add_test(NAME Test_Z8_Variant_2 COMMAND test_runner variant_2)
# add_test(NAME Test_Z8_X1 COMMAND test_runner X1)
add_test(NAME Test_Z11_D1 COMMAND test_runner D1)
