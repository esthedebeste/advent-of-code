cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 26)
project(advent-of-code CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/shared)
file(GLOB_RECURSE shared_sources ${CMAKE_CURRENT_SOURCE_DIR}/shared/*.h)

file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.3/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=2e89a807e906eab270c68c531129dc6b7f989c79a38a4cca7dde0f3ec93d9c68
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

find_package(PkgConfig)
pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)
link_libraries(${mpfr_LINK_LIBRARIES})
include_directories(${mpfr_INCLUDE_DIRS})
CPMAddPackage(
        NAME thread-pool
        GITHUB_REPOSITORY DeveloperPaul123/thread-pool
        GIT_TAG 2f75b94
        OPTIONS
        "TP_BUILD_TESTS OFF"
        "TP_BUILD_BENCHMARKS OFF"
        "TP_BUILD_EXAMPLES OFF"
)
link_libraries(dp::thread-pool)
include_directories(${thread-pool_INCLUDE_DIRS})

function(add_year year min_day max_day)
  foreach (day RANGE ${min_day} ${max_day})
    if (day LESS 10)
      set(day "0${day}")
    endif ()

    set(defs AOC_YEAR=${year} AOC_DAY=${day})

    if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
      file(READ ${CMAKE_CURRENT_SOURCE_DIR}/input/${year}/${day}.txt input)
      file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/input/${year}/${day}.cpp "")
      while (NOT input STREQUAL "")
        string(LENGTH "${input}" len)
        if (len GREATER 2048) # MSVC has a limit of ~2048 characters per string literal. This is a workaround.
          string(SUBSTRING "${input}" 0 2048 line)
          string(SUBSTRING "${input}" 2048 -1 input)
        else ()
          set(line ${input})
          set(input "")
        endif ()
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/input/${year}/${day}.cpp "R\"==(${line})==\"\n")
      endwhile ()
      set(defs ${defs} AOC_INPUT_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/input/${year}/${day}.cpp\")
    endif ()

    add_executable(${year}-${day} ${CMAKE_CURRENT_SOURCE_DIR}/days/${year}/${day}.cpp ${shared_sources})
    target_compile_definitions(${year}-${day} PRIVATE ${defs})
  endforeach ()
endfunction()

add_year(2021 1 8)
add_year(2022 1 21)
add_year(2023 1 6)
add_year(2024 1 22)