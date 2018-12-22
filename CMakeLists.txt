cmake_minimum_required(VERSION 3.12...3.13)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(Spiral VERSION 1.0 DESCRIPTION "Spiral JIT" LANGUAGES CXX)

add_executable(spiraljit_tests src/spiral.cpp)
target_include_directories(spiraljit_tests PUBLIC include)
target_compile_features(spiraljit_tests PRIVATE cxx_std_17)

if(MSVC)
    add_compile_options("/W4" "$<$<CONFIG:RELEASE>:/O2 /GR- /Gy /GL /GF /Oi /LTCG /OPT:REF /OPT:ICF>")
else()
    add_compile_options("-Wall" "-Wextra" "$<$<CONFIG:RELEASE>:-O3 -fno-rtti -flto>")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options("-stdlib=libc++")
    else()
        # nothing special for gcc at the moment
    endif()
endif()
