set(LL_LIB LL)
file(GLOB SRC "*.cpp")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
add_library(${LL_LIB} STATIC ${SRC})
target_link_libraries(${LL_LIB} PRIVATE
	back_general
)
target_include_directories(${LL_LIB} PRIVATE
	"./include"
	"${SRC_ROOT}/general/include"
)

