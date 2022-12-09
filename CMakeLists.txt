cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
project(aoc-2022 CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/shared)

foreach(day RANGE 1 8)
    add_executable(2021-${day} ${CMAKE_CURRENT_SOURCE_DIR}/days/2021/${day}.cpp)
    target_compile_definitions(2021-${day} PRIVATE AOC_YEAR=2021 AOC_DAY=${day})
endforeach()

foreach(day RANGE 1 9)
    add_executable(2022-${day} ${CMAKE_CURRENT_SOURCE_DIR}/days/2022/${day}.cpp)
    target_compile_definitions(2022-${day} PRIVATE AOC_YEAR=2022 AOC_DAY=${day})
endforeach()
