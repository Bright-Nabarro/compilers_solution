set (TARGET unit_test)
include_directories(${CMAKE_SOURCE_DIR}/src/util/include)
include_directories(./include)
aux_source_directory(. SRC)

add_executable(${TARGET} ${SRC})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
	${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(${TARGET} PRIVATE util)
target_link_libraries(${TARGET} PRIVATE gtest)
