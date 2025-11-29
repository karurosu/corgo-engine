cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)

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
file(GLOB CORE "src/core/*.c" "src/core/*.h" "src/core/**/*.c" "src/core/**/*.h")

# Tests Files
file(GLOB TESTS "src/tests/*.c" "src/tests/*.h" "src/tests/**/*.c" "src/tests/**/*.h")

# External libraries
file(GLOB EXT_LIBS "src/external/*.c" "src/external/*.h" "src/external/**/*.c" "src/external/**/*.h")

if (TOOLCHAIN STREQUAL "armgcc")
	add_compile_options(-Wdouble-promotion -Werror)
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		add_executable(${PLAYDATE_GAME_DEVICE} src/main.c ${EXT_LIBS} ${CORE})
	else()
		add_executable(${PLAYDATE_GAME_DEVICE} src/main.c ${EXT_LIBS} ${CORE})
	endif()
else()
	add_compile_options(-WX)
	add_library(${PLAYDATE_GAME_NAME} SHARED src/main.c ${EXT_LIBS} ${CORE})
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)

