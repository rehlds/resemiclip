#----------------------------------------
# 1. Preparing build:
#    rm -rf build
#    mkdir build && cd build
#
# 2. Select compiler and build it
# - Compile with Clang:
#    CC="clang" CXX="clang++" cmake ..
#    make
#
# - Compile with Intel C++ Compiler:
#    CC="icc" CXX="icpc" cmake ..
#    make
#
# - Compile with GCC Compiler:
#    cmake ..
#    make
#----------------------------------------

cmake_minimum_required(VERSION 3.1)
project(resemiclip CXX)

option(DEBUG "Build with debug information." OFF)
option(USE_STATIC_LIBSTDC "Enables static linking libstdc++." OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Avoid -fPIC option
set(CMAKE_SHARED_LIBRARY_CXX_FLAGS "")

set(COMPILE_FLAGS "-m32 -U_FORTIFY_SOURCE")
set(LINK_FLAGS "-m32 -s")

set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wall -fno-exceptions -fno-builtin -Wno-unknown-pragmas")

# Remove noxref code and data
set(COMPILE_FLAGS "${COMPILE_FLAGS} -ffunction-sections -fdata-sections")

if (DEBUG)
	set(COMPILE_FLAGS "${COMPILE_FLAGS} -g3 -O3 -ggdb")
else()
	set(COMPILE_FLAGS "${COMPILE_FLAGS} -g0 -O3 -fno-stack-protector")
endif()

# Check Intel C++ compiler
if ("$ENV{CXX}" MATCHES "icpc")
	#
	# -fp-model=precise
	# ICC uses -fp-model fast=1 by default for more aggressive optimizations on floating-point calculations
	# https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/compiler-options/compiler-option-details/floating-point-options/fp-model-fp.html#fp-model-fp_GUID-99936BBA-1508-4E9F-AC09-FA98613CE2F5
	#
	set(COMPILE_FLAGS "${COMPILE_FLAGS} \
		-fp-model=precise\
		-Qoption,cpp,--treat_func_as_string_literal_cpp\
		-inline-forceinline\
		-no-ansi-alias")

	set(LINK_FLAGS "${LINK_FLAGS} \
		-static-intel\
		-no-intel-extensions")

	if (NOT DEBUG)
		set(COMPILE_FLAGS "${COMPILE_FLAGS} -ipo")
		set(LINK_FLAGS "${LINK_FLAGS} -ipo")
	endif()
else()
	# Produce code optimized for the most common IA32/AMD64/EM64T processors.
	# As new processors are deployed in the marketplace, the behavior of this option will change.
	set(COMPILE_FLAGS "${COMPILE_FLAGS} \
		-mtune=generic -msse3\
		-Wno-write-strings\
		-fno-sized-deallocation -Wno-strict-aliasing")
endif()

# GCC >= 8.3
if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
	set(COMPILE_FLAGS "${COMPILE_FLAGS} -fcf-protection=none")
endif()

if (NOT DEBUG)
	set(LINK_FLAGS "${LINK_FLAGS} \
		-Wl,-gc-sections -Wl,--version-script=\"${PROJECT_SOURCE_DIR}/version_script.lds\"")
endif()

set(PROJECT_SRC_DIR
	"${PROJECT_SOURCE_DIR}/"
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/version"
)

set(PROJECT_CSSDK_DIR
	"${PROJECT_SOURCE_DIR}/cssdk/common"
	"${PROJECT_SOURCE_DIR}/cssdk/dlls"
	"${PROJECT_SOURCE_DIR}/cssdk/engine"
	"${PROJECT_SOURCE_DIR}/cssdk/game_shared"
	"${PROJECT_SOURCE_DIR}/cssdk/pm_shared"
	"${PROJECT_SOURCE_DIR}/cssdk/public"
)

set(PROJECT_METAMOD_DIR
	"${PROJECT_SOURCE_DIR}/metamod"
)

set(MAIN_SRCS
	"src/precompiled.cpp"
	"src/h_export.cpp"
	"src/gamedll_api.cpp"
	"src/engine_rehlds_api.cpp"
	"src/meta_api.cpp"
	"src/main.cpp"
	"src/config.cpp"
)

set(PUBLIC_SRCS
	"cssdk/public/interface.cpp"
)

add_library(resemiclip SHARED ${appversion.sh})

if (NOT TARGET appversion)
	add_custom_target(appversion DEPENDS COMMAND "${PROJECT_SOURCE_DIR}/version/appversion.sh" "${PROJECT_SOURCE_DIR}/")
endif()

add_dependencies(resemiclip appversion)

target_include_directories(resemiclip PRIVATE
	${PROJECT_SRC_DIR}
	${PROJECT_CSSDK_DIR}
	${PROJECT_METAMOD_DIR}
)

target_compile_definitions(resemiclip PRIVATE
	_LINUX
	LINUX
	NDEBUG
	_GLIBCXX_USE_CXX11_ABI=0
	HAVE_STRONG_TYPEDEF
	_stricmp=strcasecmp
	_strnicmp=strncasecmp
	_vsnprintf=vsnprintf
	_snprintf=snprintf
)

target_sources(resemiclip PRIVATE
	${MAIN_SRCS}
	${PUBLIC_SRCS}
)

target_link_libraries(resemiclip PRIVATE
	dl
)

if (USE_STATIC_LIBSTDC)
	target_compile_definitions(resemiclip PRIVATE BUILD_STATIC_LIBSTDC)
	set(LINK_FLAGS "${LINK_FLAGS} -static-libgcc -static-libstdc++")
endif()

set(LINK_FLAGS "${LINK_FLAGS} \
	-Wl,-rpath,'$ORIGIN/.' \
	-L${PROJECT_SOURCE_DIR}/lib/linux32")

set_target_properties(resemiclip PROPERTIES
	OUTPUT_NAME resemiclip_mm_i386
	PREFIX ""
	COMPILE_FLAGS ${COMPILE_FLAGS}
	LINK_FLAGS ${LINK_FLAGS}
	POSITION_INDEPENDENT_CODE OFF
)
