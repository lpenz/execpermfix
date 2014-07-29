
cmake_minimum_required (VERSION 2.6)

project (execpermfix)

add_executable(execpermfix execpermfix.c)

add_custom_command(
	OUTPUT execpermfix.1
	DEPENDS manual.t2t
	COMMAND txt2tags -q -t man -i ${CMAKE_CURRENT_SOURCE_DIR}/manual.t2t -o ${CMAKE_CURRENT_BINARY_DIR}/execpermfix.1
	)
add_custom_target(manual ALL DEPENDS execpermfix.1)

