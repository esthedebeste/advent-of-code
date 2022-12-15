cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
project(aoc-2022 CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/shared)

function(add_year year min_day max_day)
    foreach(day RANGE ${min_day} ${max_day})
        if(day LESS 10)
            set(day "0${day}")
        endif()

        add_executable(${year}-${day} ${CMAKE_CURRENT_SOURCE_DIR}/days/${year}/${day}.cpp)
        target_compile_definitions(${year}-${day} PRIVATE AOC_YEAR=${year} AOC_DAY=${day})
    endforeach()
endfunction()

add_year(2021 1 8)
add_year(2022 1 15)