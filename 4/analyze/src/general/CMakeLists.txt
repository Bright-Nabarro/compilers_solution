set(GENERAL_LIB back_general)
file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

# 添加yaml-cpp库
include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 0.8.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)

FetchContent_MakeAvailable(yaml-cpp)

add_library(${GENERAL_LIB} STATIC ${SRC})

target_link_libraries(${GENERAL_LIB} PRIVATE yaml-cpp) # The library or executable that require yaml-cpp library

target_include_directories(${GENERAL_LIB}
	PRIVATE "./include"
)

