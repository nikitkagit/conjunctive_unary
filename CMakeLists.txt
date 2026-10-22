cmake_minimum_required(VERSION 3.16)

project(conjuctive_unary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(conjuctive_unary_library
    src/equation.cpp
    src/intersection_expression.cpp
    src/sum_expression.cpp
    src/union_expression.cpp
    src/variable_expression.cpp
    src/expression_proxy.cpp
    src/constant_expression.cpp
    src/equation_calculator.cpp
    src/variable_dictionary.cpp
    src/parser.cpp
)
target_include_directories(conjuctive_unary_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CU
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(gcu
    example/main.cpp
)
target_link_libraries(gcu PRIVATE conjuctive_unary_library)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(conjuctive_unary_library PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(gcu PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(conjuctive_unary_library PRIVATE /W4)
    target_compile_options(gcu PRIVATE /W4)
endif()

enable_testing()

add_executable(expressions_test
    test/expressions_test.cpp
)
target_link_libraries(expressions_test PRIVATE conjuctive_unary_library)
add_test(NAME expressions_test COMMAND expressions_test)

add_executable(equation_calculator_test
    test/equation_calculator_test.cpp
)
target_link_libraries(equation_calculator_test PRIVATE conjuctive_unary_library)
add_test(NAME equation_calculator_test COMMAND equation_calculator_test)

add_executable(parser_test
    test/parser_test.cpp
)
target_link_libraries(parser_test PRIVATE conjuctive_unary_library)
add_test(NAME parser_test COMMAND parser_test)