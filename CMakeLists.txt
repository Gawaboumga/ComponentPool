cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")

project("Component Pool")

set(CP_INCLUDE_DIR include/)
set(CP_EXTLIBS_DIR extlibs/)
set(CP_SOURCE_TEST_DIR tests/)

include_directories(
		${CP_INCLUDE_DIR}
		${CP_EXTLIBS_DIR}
	)

file(GLOB TESTS_SOURCE
		${CP_EXTLIBS_DIR}Catch/*.cpp
		${CP_SOURCE_TEST_DIR}*.cpp
	)

set(EXEC_NAME UnitTests)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../tests/bin/)

add_executable(${EXEC_NAME} ${TESTS_SOURCE})
