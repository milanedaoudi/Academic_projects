cmake_minimum_required(VERSION 2.8)
project(3A-SoC-Cpp )
# ----------------------------
# Options de build
# ----------------------------
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wconversion")

# ----------------------------
# Inclure les headers
# ----------------------------
include_directories(${PROJECT_SOURCE_DIR}/include)

# ----------------------------
# 1. Créer la bibliothèque commune (tout sauf les mains)
# ----------------------------
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Exclure les fichiers main.cpp et main2.cpp
list(FILTER SRC_FILES EXCLUDE REGEX "src/main\\.cpp$")
list(FILTER SRC_FILES EXCLUDE REGEX "src/main2\\.cpp$")

# Crée la librairie partagée contenant le reste du code
add_library(project_lib ${SRC_FILES})

#------------------------------
# 2. Exécutable principal global
# ----------------------------
add_executable(main_exe src/main.cpp)
target_link_libraries(main_exe project_lib)

add_executable(main_exe2 src/main2.cpp)
target_link_libraries(main_exe2 project_lib)

# ----------------------------
# 3. Tests
# ----------------------------
enable_testing()
file(GLOB TEST_SOURCES "test/*.cpp")

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src} ${SOURCES})
    target_link_libraries(${test_name} project_lib)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

