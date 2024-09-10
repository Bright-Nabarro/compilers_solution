set (TARGET app)

aux_source_directory(. SRC)
include_directories(./include)
include_directories(${CMAKE_SOURCE_DIR}/src/main)

add_executable(${TARGET} ${SRC})
target_link_libraries(${TARGET} PRIVATE boost_program_options)
target_link_libraries(${TARGET} PRIVATE util)

