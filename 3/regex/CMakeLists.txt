cmake_minimum_required(VERSION 3.10)

project(regex_praser CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (ENABLE_RELEASE)
	set(CMAKE_BUILD_TYPE Release)
	add_compile_definitions(NDEBUG)
else()
	set(CMAKE_BUILD_TYPE Debug)
	add_compile_definitions(DEBUG)
endif()

add_compile_options(
	-Wall
	-Wextra
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(SRC_ROOT ${CMAKE_SOURCE_DIR}/src)

add_subdirectory("src/main")
add_subdirectory("src/util")
add_subdirectory("test")

#将clangd_custom_target复制到CMakeLists.txt所在目录
add_custom_target(move_compile_commands ALL
	COMMAND ${CMAKE_COMMAND} -E copy_if_different			#平台无关的复制命令
			"${CMAKE_BINARY_DIR}/compile_commands.json"
			"${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json"
	DEPENDS "${CMAKE_BINARY_DIR}/compile_commands.json"
)

