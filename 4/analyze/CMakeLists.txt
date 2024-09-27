cmake_minimum_required(VERSION 3.10)

project(analyze LANGUAGES CXX)

option(ENABLE_RELEASE "Enable release build" OFF)
option(ENABLE_TESTING "Enable testing" ON)

if (ENABLE_RELEASE)
	set(CMAKE_BUILD_TYPE Release)
	add_compile_definitions(NDBUG)
	add_compile_options(
		-O2
	)
else()
	set(CMAKE_BUILD_TYPE Debug)
	add_compile_definitions(DEBUG)
	add_compile_options(
		-O0
		-g3
	)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
	-Wall
	-Wextra
)

find_package(Boost 1.65 REQUIRED COMPONENTS program_options)
if (Boost_FOUND)
	message(STATUS "Boost version: ${Boost_VERSION}")
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
else()
	message(FATAL_ERROR "Boost libraries not found!")
endif()


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(
		-stdlib=libc++
	)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/W4)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(SRC_ROOT ${CMAKE_SOURCE_DIR}/src)

if (ENABLE_TESTING)
	find_package(GTest REQUIRED)
	if (GTest_FOUND)
		include_directories(${GTest_INCLUDE_DIRS})
	else()
		message(FATAL_ERROR "Google Test not found")
	endif()
	add_subdirectory("test")
endif()

# 将compile_commands.json复制到源目录
add_custom_target(move_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json"
    DEPENDS "${CMAKE_BINARY_DIR}/compile_commands.json"
)

