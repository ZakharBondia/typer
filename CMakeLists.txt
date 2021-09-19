cmake_minimum_required(VERSION 3.13.0)
project(typer VERSION 0.1.0)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# lib
add_library(typer INTERFACE)
add_library(Typer::Typer ALIAS typer)

target_include_directories(typer INTERFACE include)
target_sources(typer INTERFACE 
    include/typer/declaration.hpp
    include/typer/introspection.hpp
    include/typer/extra/equal.hpp
    include/typer/extra/equal_op.hpp
    include/typer/extra/hash.hpp
)

#3rd party
add_library(Catch2 INTERFACE)
target_include_directories(Catch2 INTERFACE 3rd_party)
add_library(Json INTERFACE)
target_include_directories(Json INTERFACE 3rd_party)

#tests
add_executable(typer_test)
add_test(typer_test typer_test)
target_sources(typer_test
 PRIVATE 
    test/introspection.test.cpp
    test/hash.test.cpp
    test/print.test.cpp
    test/equal.test.cpp
    test/nlohmann_json.test.cpp
    
)
target_link_libraries(typer_test PRIVATE Typer::Typer Catch2 Json)

