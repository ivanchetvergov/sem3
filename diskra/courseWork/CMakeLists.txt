cmake_minimum_required(VERSION 3.10)
project(FiniteCalculator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_BUILD_TYPE Debug)

include_directories(core/include)

# фикс ворнинга pybind11
cmake_policy(SET CMP0148 NEW)
find_package(Python 3 COMPONENTS Interpreter Development)

find_package(pybind11 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(GTest REQUIRED) 

set(CORE_SOURCES
    core/src/RingNumber.cc
    core/src/FiniteRingRules.cc
    core/src/SmallRingArithmetic.cc 
    core/src/BigRingArithmetic.cc
)

# модуль для python
pybind11_add_module(finite_ring_module
    ${CORE_SOURCES}
    core/py_binding.cc
)
target_link_libraries(finite_ring_module PRIVATE yaml-cpp::yaml-cpp)


# --- GTest / CTest ИНТЕГРАЦИЯ ---


# enable_testing()

# add_executable(
#     unit_tests            
#     ${CORE_SOURCES}      
#     tests/gtest.cc
# )

# target_link_libraries(
#     unit_tests             
#     PRIVATE 
#     yaml-cpp::yaml-cpp
#     GTest::gtest_main      
# )

# gtest_discover_tests(unit_tests)
# enable_testing()
# add_test(NAME Test_Z8_Variant_1 COMMAND test_runner variant_1)
# add_test(NAME Test_Z8_Variant_2 COMMAND test_runner variant_2)
# # add_test(NAME Test_Z8_X1 COMMAND test_runner X1)
# add_test(NAME Test_Z11_D1 COMMAND test_runner D1)
