cmake_minimum_required(VERSION 3.28)
project(Etudes VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif()

########################################
# Dependencies #########################
########################################

find_package(astroutils REQUIRED)
find_package(OpenMP REQUIRED)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

find_program(MOLD_PROGRAM mold)
if(MOLD_PROGRAM)
  add_link_options(-fuse-ld=mold)
endif()

find_package(fmt CONFIG REQUIRED)

########################################
## Executable ##########################
########################################

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(etudes ${SOURCES})
target_precompile_headers(etudes PRIVATE src/pch.h)
target_include_directories(etudes PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_options(etudes PRIVATE
  -Wall -Wextra -Wpedantic -fopenmp -fsized-deallocation
  $<$<CXX_COMPILER_ID:GNU,Clang>:-fdiagnostics-color=always>)
target_compile_definitions(etudes PRIVATE
  NAME="${PROJECT_NAME}"
  VERSION="${PROJECT_VERSION}")
target_link_libraries(etudes PRIVATE
  astroutils::astroutils
  fmt::fmt
  OpenMP::OpenMP_CXX)

########################################
## Sanitizers ##########################
########################################

# include(cmake/AdddessAndUbSanitizers.cmake)
# include(cmake/ThreadSanitizer.cmake)