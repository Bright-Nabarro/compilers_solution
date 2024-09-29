set(LL1_LIB LL1)
file(GLOB SRC "*.cpp")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
add_library(${LL1_LIB} STATIC ${SRC})

target_include_directories(${LL1_LIB}
	PRIVATE "./include"
)

