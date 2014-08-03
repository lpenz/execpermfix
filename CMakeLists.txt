
cmake_minimum_required (VERSION 2.6)

project (execpermfix)

# Executable:
add_executable(execpermfix src/execpermfix.c)

# Manual:
add_custom_command(
	OUTPUT execpermfix.1
	DEPENDS manual.t2t
	COMMAND txt2tags -q -t man -i ${CMAKE_CURRENT_SOURCE_DIR}/manual.t2t -o ${CMAKE_CURRENT_BINARY_DIR}/execpermfix.1
	)
add_custom_target(manual ALL DEPENDS execpermfix.1)


# Tests:
enable_testing()

add_test (NAME test_execpermfix
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/test_execpermfix)

