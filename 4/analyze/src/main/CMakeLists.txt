set(APP_TARGET app)
file(GLOB SRC "*.cpp")

add_executable(${APP_TARGET} ${SRC})
target_include_directories(${APP_TARGET}
	PRIVATE ${Boost_INCLUDE_DIRS}
	PRIVATE "${SRC_ROOT}/LL-1/include"
)

target_link_libraries(${APP_TARGET}
	PRIVATE Boost::program_options
	PRIVATE LL-1
)

