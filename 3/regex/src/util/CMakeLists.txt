set (TARGET util)
aux_source_directory(. SRC)
include_directories(./include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
add_library(${TARGET} STATIC ${SRC})

