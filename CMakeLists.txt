project(sid-play)
cmake_minimum_required(VERSION 3.12)
set( EXECUTABLE_OUTPUT_PATH ../bin)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin)

add_link_options(-s -Wl,--gc-sections -mwindows)

link_libraries(-lwin32hlp -lvgxemu -lstdshit)

add_library(chip-debug STATIC
	src/c64sid/c64dbg.cc
	src/c64sid/resource.rc
	src/ym2612/ym2612.cc
	src/ym2612/resource.rc	
	src/chipdbg.cc
)

add_custom_command(TARGET chip-debug POST_BUILD COMMAND 
	libres $<TARGET_FILE:chip-debug> $<TARGET_FILE:chip-debug>)
	


target_link_options(chip-debug PRIVATE -nostartfiles)


link_libraries(chip-debug)

add_executable(test 
	src/test/resource.rc
	src/test/test.cc
)




