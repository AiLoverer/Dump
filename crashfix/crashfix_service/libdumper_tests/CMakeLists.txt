cmake_minimum_required(VERSION 2.8)
project(libdumper_tests)

include_directories("${CMAKE_SOURCE_DIR}/libdumper" "${CMAKE_SOURCE_DIR}/libtest")
aux_source_directory( . source_files)
add_definitions( -D_UNICODE )

# In Windows, use UNICODE
if(WIN32)
add_definitions(-D_UNICODE -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

include_directories( ${CMAKE_SOURCE_DIR}/libdumper )

if(UNIX)
	ADD_PCH_RULE("stdafx.cpp"  source_files)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl")
else(UNIX)
	ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" source_files)	
endif(UNIX)

add_executable(libdumper_tests ${source_files})
if(WIN32)
	target_link_libraries(libdumper_tests libdumper libeay32.lib libtest)
else(WIN32)
	target_link_libraries(libdumper_tests libdumper libtest ${CMAKE_DL_LIBS})
endif(WIN32)

