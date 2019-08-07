project(dumper)

aux_source_directory( . source_files )
add_definitions(-D_UNICODE -D_CRT_SECURE_NO_DEPRECATE)

include_directories( ${CMAKE_SOURCE_DIR}/libdumper )

if(UNIX)
	ADD_PCH_RULE("stdafx.cpp"  source_files)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl")
else(UNIX)
	ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" source_files)
endif(UNIX)

add_executable(dumper ${source_files})
target_link_libraries(dumper libdumper)

set_target_properties(dumper PROPERTIES RUNTIME_OUTPUT_DIRECTORY_Debug "${CMAKE_BINARY_DIR}/bin")
set_target_properties(dumper PROPERTIES RUNTIME_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/bin")
