project(minizip)

# Create the list of source files
aux_source_directory( . source_files )

if(UNIX)
	list(REMOVE_ITEM source_files ./iowin32.c )
endif(UNIX)

# Define _UNICODE (use wide-char encoding)
add_definitions(-D_UNICODE -D_CRT_SECURE_NO_DEPRECATE)

if(WIN32)
	add_definitions(/wd4996)
endif(WIN32)

include_directories( ${CMAKE_SOURCE_DIR}/thirdparty/zlib)

# Add library build target
add_library(minizip ${source_files})

set_target_properties(minizip PROPERTIES DEBUG_POSTFIX d )

target_link_libraries(minizip zlib)
