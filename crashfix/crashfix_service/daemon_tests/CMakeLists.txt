project(daemon_tests)

aux_source_directory( . source_files )

include_directories( ${CMAKE_SOURCE_DIR}/libdumper ${CMAKE_SOURCE_DIR}/daemon ${CMAKE_SOURCE_DIR}/libtest )

if(UNIX)
	ADD_PCH_RULE("stdafx.cpp"  source_files)
else(UNIX)
	ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" source_files)
endif(UNIX)

add_executable(daemon_tests ${source_files})
target_link_libraries(daemon_tests libdumper libtest)
if(WIN32)
target_link_libraries(daemon_tests ws2_32.lib libtest)
endif(WIN32)

