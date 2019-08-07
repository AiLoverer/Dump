project(libdumper)
cmake_minimum_required(VERSION 2.8)

# Create the list of source files to compile
aux_source_directory( . source_files)

# In Windows, use UNICODE
if(WIN32)
add_definitions(-D_UNICODE -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

# In Linux, exclude WIN32-specific sources from build
if(UNIX)
	list(REMOVE_ITEM source_files ./DbgHelpDumpWriter.cpp ./MiniDumpWriterImpl.cpp
                           ./RegProfile.cpp ./HttpRequestSender.cpp ./AssyncNotification.cpp 
                           ./Uploader.cpp)
endif(UNIX)

# Enable precompiled headers to improve compilation speed
if(UNIX)
	ADD_PCH_RULE("stdafx.cpp"  source_files)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl")
else(UNIX)
	ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" source_files)
endif(UNIX)

# Add build target
add_library(libdumper ${source_files})

# Override library name when compiling in Debug mode
set_target_properties(libdumper PROPERTIES DEBUG_POSTFIX d)

# Specify link dependencies for the target
target_link_libraries(libdumper zlib minizip tinyxml)

# In Linux, link to pthread library
if(UNIX)	
	target_link_libraries(libdumper pthread)
	set_target_properties(libdumper PROPERTIES COMPILE_FLAGS "-lrt")
endif(UNIX)

