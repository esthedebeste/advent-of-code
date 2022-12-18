cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
project(aoc-2022 CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/shared)

function(add_year year min_day max_day)
    foreach(day RANGE ${min_day} ${max_day})
        if(day LESS 10)
            set(day "0${day}")
        endif()

        set(defs AOC_YEAR=${year} AOC_DAY=${day})

        if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
            file(READ ${CMAKE_CURRENT_SOURCE_DIR}/input/${year}/${day}.txt input)
            file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/input/${year}/${day}.cpp "R\"==(${input})==\"")
            set(defs ${defs} AOC_INPUT_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/input/${year}/${day}.cpp\")
        endif()

        add_executable(${year}-${day} ${CMAKE_CURRENT_SOURCE_DIR}/days/${year}/${day}.cpp)
        target_compile_definitions(${year}-${day} PRIVATE ${defs})
    endforeach()
endfunction()

add_year(2021 1 8)
add_year(2022 1 17)