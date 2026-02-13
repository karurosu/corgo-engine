cmake_minimum_required(VERSION 3.24)
set(CMAKE_C_STANDARD 11)

# Fetch third-party dependencies
include(FetchContent)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})

if (NOT ${ENVSDK} STREQUAL "")
	# Convert path from Windows
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
	execute_process(
			COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
			COMMAND head -n 1
			COMMAND cut -c9-
			OUTPUT_VARIABLE SDK
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if (NOT EXISTS ${SDK})
	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
	return()
endif()

FetchContent_Declare(
  cc
  GIT_REPOSITORY https://github.com/JacksonAllan/CC.git
  GIT_TAG main
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdParty/cc
)
FetchContent_MakeAvailable(cc)

FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG master
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdParty/unity
)
FetchContent_MakeAvailable(unity)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# Game Name Customization
set(PLAYDATE_GAME_NAME corgo_engine)

project(${PLAYDATE_GAME_NAME} C ASM)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	set(PLAYDATE_GAME_DEVICE corgo_engine_RELEASE)
else()
	set(PLAYDATE_GAME_DEVICE corgo_engine_DEVICE)
endif()

# Core Files
file(GLOB CORE "src/ecs/*.c" "src/ecs/*.h" "src/ecs/**/*.c" "src/ecs/**/*.h" "src/ecs/**/**/*.c" "src/ecs/**/**/*.h")

# Utils Files
file(GLOB UTILS "src/utils/*.c" "src/utils/*.h")

# Engine Files
file(GLOB ENGINE "src/engine/*.c" "src/engine/*.h" "src/engine/**/*.c" "src/engine/**/*.h")

# Game Files
file(GLOB GAME "src/game/*.c" "src/game/*.h" "src/game/**/*.c" "src/game/**/*.h")

# Tests Files
file(GLOB TESTS "src/tests/*.c" "src/tests/*.h" "src/tests/**/*.c" "src/tests/**/*.h")

# External libraries
file(GLOB EXT_LIBS "src/external/*.c" "src/external/*.h" "src/external/**/*.c" "src/external/**/*.h")

# Preserve directory structure in Visual Studio solution
source_group(TREE "${CMAKE_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${CORE} ${UTILS} ${ENGINE} ${GAME} ${EXT_LIBS})

# global compile definitions
add_compile_definitions(CC_NO_SHORT_NAMES)

# debug build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(CE_DEBUG_BUILD)
endif()

# enable warnings as errors
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# Test executable (only build for simulator/host, not device)
if (NOT TOOLCHAIN STREQUAL "armgcc")
	enable_testing()
	
	# Unity test framework library
	add_library(unity_lib STATIC ${unity_SOURCE_DIR}/src/unity.c)
	target_include_directories(unity_lib PUBLIC ${unity_SOURCE_DIR}/src)
	
	# Disable warnings for Unity (third-party code)
	if(MSVC)
		target_compile_options(unity_lib PRIVATE /wd5045)
	endif()
	
	# Core tests executable
	add_executable(coretests 
		src/tests/core.c
		${CORE}
		${UTILS}
		${ENGINE}
		${EXT_LIBS}
	)
	
	# Preserve directory structure for test project
	source_group(TREE "${CMAKE_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${CORE} ${ENGINE} ${UTILS} ${EXT_LIBS} src/tests/core.c)
	
	target_include_directories(coretests PRIVATE 
		${cc_SOURCE_DIR}
		${unity_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/src
	)
	target_link_libraries(coretests PRIVATE unity_lib)
	
	# Disable warnings for test executable
	if(MSVC)
		target_compile_options(coretests PRIVATE /wd5045)
	endif()
	
	target_compile_definitions(coretests PRIVATE CE_CORE_TEST_MODE)

	# Register test with CTest
	add_test(NAME CoreTests COMMAND coretests)
endif()

if (TOOLCHAIN STREQUAL "armgcc")
	add_executable(${PLAYDATE_GAME_DEVICE} src/main.c ${EXT_LIBS} ${CORE} ${UTILS} ${ENGINE} ${GAME})
	target_include_directories(${PLAYDATE_GAME_DEVICE} PRIVATE ${cc_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src)
	target_compile_definitions(${PLAYDATE_GAME_DEVICE} PRIVATE CE_BACKEND_PLAYDATE)
	target_compile_definitions(${PLAYDATE_GAME_DEVICE} PRIVATE CE_ARM_BUILD)
else()
	add_library(${PLAYDATE_GAME_NAME} SHARED src/main.c ${EXT_LIBS} ${CORE} ${UTILS} ${ENGINE} ${GAME})
	target_include_directories(${PLAYDATE_GAME_NAME} PRIVATE ${cc_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src)
	target_compile_definitions(${PLAYDATE_GAME_NAME} PRIVATE CE_BACKEND_PLAYDATE)
endif()

# Generate build info header at build time
set(BUILD_INFO_TEMPLATE "${CMAKE_SOURCE_DIR}/src/build_info.h.in")
set(BUILD_INFO_HEADER "${CMAKE_BINARY_DIR}/build_info.h")
set(BUILD_INFO_TIMESTAMP "${CMAKE_BINARY_DIR}/build_info.timestamp")

# This target always runs because it has no OUTPUT
add_custom_target(generate_build_info_always
	COMMAND ${CMAKE_COMMAND}
		-DINPUT_FILE=${BUILD_INFO_TEMPLATE}
		-DOUTPUT_FILE=${BUILD_INFO_HEADER}
		-P ${CMAKE_SOURCE_DIR}/generate_build_info.cmake
	COMMAND ${CMAKE_COMMAND} -E touch ${BUILD_INFO_TIMESTAMP}
	BYPRODUCTS ${BUILD_INFO_HEADER}
	COMMENT "Generating build timestamp"
	VERBATIM
)

# Make this run every time by depending on a target, not a file
add_custom_target(generate_build_info ALL DEPENDS generate_build_info_always)

# Add the generated header directory to include paths and make targets depend on it
if (TOOLCHAIN STREQUAL "armgcc")
	target_include_directories(${PLAYDATE_GAME_DEVICE} PRIVATE ${CMAKE_BINARY_DIR})
	add_dependencies(${PLAYDATE_GAME_DEVICE} generate_build_info)
else()
	target_include_directories(${PLAYDATE_GAME_NAME} PRIVATE ${CMAKE_BINARY_DIR})
	add_dependencies(${PLAYDATE_GAME_NAME} generate_build_info)
	if(TARGET coretests)
		target_include_directories(coretests PRIVATE ${CMAKE_BINARY_DIR})
		add_dependencies(coretests generate_build_info)
	endif()
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)

# Convenience target to launch the Playdate Simulator (simulator builds only)
if (NOT TOOLCHAIN STREQUAL "armgcc")
	add_custom_target(run
		COMMAND "${SDK}/bin/PlaydateSimulator.exe" "${CMAKE_SOURCE_DIR}/${PLAYDATE_GAME_NAME}.pdx"
		DEPENDS ${PLAYDATE_GAME_NAME}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Launching Playdate Simulator with ${PLAYDATE_GAME_NAME}.pdx"
		VERBATIM
	)
endif()
