cmake_minimum_required(VERSION 3.10)
project(FiniteCalculator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_BUILD_TYPE Debug)

include_directories(core/include)

find_package(pybind11 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(GTest REQUIRED) 

# модуль для python
pybind11_add_module(finite_field_module
    core/src/FiniteRing.cc
    core/src/FiniteRingRules.cc
    core/py_binding.cc
)
target_link_libraries(finite_field_module PRIVATE yaml-cpp::yaml-cpp)


# --- GTest / CTest ИНТЕГРАЦИЯ ---

# 1. создаем исполняемый файл для тестов
add_executable(
    t
    core/src/FiniteRing.cc
    core/src/FiniteRingRules.cc
    tests/gtest.cc
)

# 2. линкуем с библиотеками GTest и зависимостями
target_link_libraries(
    t
    PRIVATE 
    yaml-cpp::yaml-cpp
    GTest::gtest_main 
)

# 3. регистрируем тесты в CTest
enable_testing()

gtest_discover_tests(t)
enable_testing()
# add_test(NAME Test_Z8_Variant_1 COMMAND test_runner variant_1)
# add_test(NAME Test_Z8_Variant_2 COMMAND test_runner variant_2)
# # add_test(NAME Test_Z8_X1 COMMAND test_runner X1)
# add_test(NAME Test_Z11_D1 COMMAND test_runner D1)
