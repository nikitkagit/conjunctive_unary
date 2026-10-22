cmake_minimum_required(VERSION 3.16)

project(conjunctive_unary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(GNUInstallDirs)

# --- Options ---

# Build the example application (installed only if ON).
option(CONJUNCTIVE_UNARY_BUILD_APP "Build conjunctive_unary example application" ON)

# Build the test suite. Defaults to ON only when the library is built as the
# top-level project; disabled automatically when embedded via
# add_subdirectory()/FetchContent.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(_cu_tests_default ON)
else()
    set(_cu_tests_default OFF)
endif()
option(CONJUNCTIVE_UNARY_BUILD_TESTS "Build conjunctive_unary tests" ${_cu_tests_default})
unset(_cu_tests_default)

# --- Library target ---

add_library(conjunctive_unary_library
    src/application/application.cpp
    src/application/options.cpp
    src/application/result_printer.cpp

    src/equation/equation.cpp
    src/equation/equation_calculator.cpp
    src/equation/parser.cpp
    src/equation/variable_dictionary.cpp

    src/expression/intersection_expression.cpp
    src/expression/sum_expression.cpp
    src/expression/union_expression.cpp
    src/expression/variable_expression.cpp
    src/expression/expression_proxy.cpp
    src/expression/constant_expression.cpp
)

target_include_directories(conjunctive_unary_library PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CU
)

target_include_directories(conjunctive_unary_library PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# --- Example application ---

if(CONJUNCTIVE_UNARY_BUILD_APP)
    add_executable(gcu src/main.cpp)
    target_link_libraries(gcu PRIVATE conjunctive_unary_library)
endif()

# --- Warnings ---

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(conjunctive_unary_library PRIVATE -Wall -Wextra -Wpedantic)
    if(TARGET gcu)
        target_compile_options(gcu PRIVATE -Wall -Wextra -Wpedantic)
    endif()
elseif(MSVC)
    target_compile_options(conjunctive_unary_library PRIVATE /W4)
    if(TARGET gcu)
        target_compile_options(gcu PRIVATE /W4)
    endif()
endif()

# --- Tests ---

if(CONJUNCTIVE_UNARY_BUILD_TESTS)
    enable_testing()

    function(add_project_test source)
        get_filename_component(name "${source}" NAME_WE)
        add_executable(${name} ${source})
        target_link_libraries(${name} PRIVATE conjunctive_unary_library)
        target_include_directories(${name} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/CU
        )
        add_test(NAME ${name} COMMAND ${name})
    endfunction()

    add_project_test(test/expressions_test.cpp)
    add_project_test(test/equation_calculator_test.cpp)
    add_project_test(test/parser_test.cpp)
    add_project_test(test/application_test.cpp)
endif()

# --- Install ---

install(TARGETS conjunctive_unary_library
    EXPORT conjunctive_unaryTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install the application only when it was built.
if(TARGET gcu)
    install(TARGETS gcu
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Tests are never installed: they are build-time artifacts, not part of the
# distributed package. Test sources remain in the repository.

install(DIRECTORY include/CU DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT conjunctive_unaryTargets
    FILE conjunctive_unaryTargets.cmake
    NAMESPACE conjunctive_unary::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/conjunctive_unary
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/conjunctive_unaryConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/conjunctive_unaryConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/conjunctive_unary
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/conjunctive_unaryConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/conjunctive_unaryConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/conjunctive_unaryConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/conjunctive_unary
)