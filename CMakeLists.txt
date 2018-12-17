cmake_minimum_required(VERSION 3.12...3.13)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(CMakeTestProj VERSION 1.0 DESCRIPTION "Very nice project" LANGUAGES CXX)

add_executable(ctest src/main.cpp)
target_include_directories(ctest PUBLIC include)
target_compile_features(ctest PRIVATE cxx_std_17)

if(MSVC)
    add_compile_options("/W4" "/std:c++17" "$<$<CONFIG:RELEASE>:/O2 /GR- /Gy /GL /GF /Oi /LTCG /OPT:REF /OPT:ICF>")
else()
    add_compile_options("-Wall" "-Wextra" "$<$<CONFIG:RELEASE>:-O3 -fno-rtti -flto>")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options("-stdlib=libc++")
    else()
        # nothing special for gcc at the moment
    endif()
endif()
