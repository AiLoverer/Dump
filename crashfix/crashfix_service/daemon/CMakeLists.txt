project(daemon)

aux_source_directory( . source_files )

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

add_executable(daemon ${source_files})
target_link_libraries(daemon libdumper ${CMAKE_DL_LIBS})
if(WIN32)
target_link_libraries(daemon ws2_32.lib libeay32.lib)
endif(WIN32)

set_target_properties(daemon PROPERTIES OUTPUT_NAME crashfixd)

if(UNIX)
install(TARGETS daemon 
        DESTINATION /usr/sbin/)
endif(UNIX)
