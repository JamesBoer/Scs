
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Set project name
project(Scs)

# Set folders based on file structure
function(scs_auto_set_folders source_list)
	foreach(source IN LISTS ${source_list})
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "/" "\\" source_path_folder "${source_path}")
		source_group("${source_path_folder}" FILES "${source}")
	endforeach()
endfunction()

# Set folders manuallly
function(scs_set_folders source_list folder_name)
	foreach(source IN LISTS source_list)
		source_group("\\${folder_name}" FILES "${source}")
	endforeach()
endfunction()

# Build a test or utility executable
function(scs_build_executable project_name source_list is_linked is_startup)

	# Create executable as project name
	add_executable(${project_name} ${source_list})

	# Set C++ 17 compiler flags
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)

	# Set the executable as the default startup project
	if(is_startup)
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${project_name})
	endif()

	# Add Scs library
	if(is_linked)
		target_link_libraries(${project_name} Scs)
	endif()

	# Create IDE folder structure
	scs_set_folders("${source_list}" Source)

endfunction()

# Enable IDE folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(
    scs_source_list
    "Source/Scs.h"
    "Source/Scs.cpp"
    "Source/ScsAddress.cpp"
    "Source/ScsAddress.h"
    "Source/ScsClient.cpp"
    "Source/ScsClient.h"
    "Source/ScsCommon.cpp"
    "Source/ScsCommon.h"
    "Source/ScsInternal.h"
    "Source/ScsReceiveQueue.cpp"
    "Source/ScsReceiveQueue.h"
    "Source/ScsSendQueue.cpp"
    "Source/ScsSendQueue.h"
    "Source/ScsServer.cpp"
    "Source/ScsServer.h"
    "Source/ScsSocket.cpp"
    "Source/ScsSocket.h"
)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create library as project name
add_library(${PROJECT_NAME} ${scs_source_list})

# Add pthreads for Linux builds
if(UNIX AND NOT APPLE)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	target_link_libraries(${PROJECT_NAME} Threads::Threads)
endif()

# Set warning level
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Create IDE folder structure
scs_auto_set_folders(scs_source_list)

# Check if Scs is included via add_subdirectory.
get_directory_property(scs_is_subproject PARENT_DIRECTORY)

# Only build the test suite and utilities if this is not a subproject
if(NOT scs_is_subproject)

	set(
		scstest_source_list
		"Tests/ScsTest/ScsTest.cpp"
	)
	set(
		unittests_source_list
		"Tests/UnitTests/catch.hpp"
		"Tests/UnitTests/Main.cpp"
		"Tests/UnitTests/TestConnection.cpp"
	)
	scs_build_executable(ScsTest "${scstest_source_list}" TRUE FALSE)
	scs_build_executable(UnitTests "${unittests_source_list}" TRUE TRUE)

endif()
