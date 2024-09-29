cmake_minimum_required(VERSION 3.10)

project(analyze LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
	-Wall
	-Wextra
)

#find_package(Boost 1.70 REQUIRED COMPONENTS program_options CONFIG)
#if (Boost_FOUND)
#	message(STATUS "Boost version: ${Boost_VERSION}")
#	else()
#	message(FATAL_ERROR "Boost libraries not found!")
#endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(
		-stdlib=libc++
	)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/W4)
endif()

include_directories(
	"./extern"
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(BIN_OUTPUT_DESTINATION ${CMAKE_SOURCE_DIR}/bin)
set(SRC_ROOT ${CMAKE_SOURCE_DIR}/src)

#add_subdirectory("src/main")
add_subdirectory("src/general")
#add_subdirectory("src/LL")

if (ENABLE_TESTING)
	find_package(GTest REQUIRED)
	if (GTest_FOUND)
		message(STATUS "Google Test version ${GTest_VERSION}")
		add_subdirectory("test")
	else()
		message(FATAL_ERROR "Google Test not found")
	endif()
endif()

# 将compile_commands.json复制到源目录
add_custom_target(move_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json"
    DEPENDS "${CMAKE_BINARY_DIR}/compile_commands.json"
)

