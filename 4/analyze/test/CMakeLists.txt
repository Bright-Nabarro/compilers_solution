set(TEST_TARGET test_app)

file(GLOB SRC "*.cpp")

link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(${TEST_TARGET} ${SRC})


target_link_libraries(${TEST_TARGET} PRIVATE
	LL
	gtest
)

target_include_directories( ${TEST_TARGET}
	#	PRIVATE "./include"
	PRIVATE "${SRC_ROOT}/general/include"
	PRIVATE "${SRC_ROOT}/LL/include"
)

add_custom_command(
	TARGET ${TEST_TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TEST_TARGET}> ${BIN_OUTPUT_DESTINATION}
	COMMENT "Copying ${TEST_TARGET} to ${BIN_OUTPUT_DESTINATION}"
)

