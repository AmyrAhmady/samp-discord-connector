cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
project(discord-connector VERSION 0.2.4)

if (NOT DCC_VERSION) # allow version override
	set(DCC_VERSION ${PROJECT_VERSION})
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/libs/cmake")

if(MSVC)
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
	set(CMAKE_BUILD_TYPE "" CACHE STRING "" FORCE)
endif()

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/artifact")
# -- CONAN
include(CMakeFiles/conan.cmake)
function(add_conan_lib pkg_name conan_pkg)
	conan_check(REQUIRED)

	conan_cmake_run(
		REQUIRES ${conan_pkg}
		ARCH x86
		BASIC_SETUP CMAKE_TARGETS
		BUILD missing
	)

	set_target_properties(${pkg_name} PROPERTIES IMPORTED_GLOBAL TRUE)
endfunction()

conan_add_remote(NAME conan-transit
                 INDEX 1
                 URL https://api.bintray.com/conan/conan/conan-transit
                 VERIFY_SSL True)
if (MSVC)
	add_link_options(/INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
	# link runtime statically
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX)
	# hide non-exported symbols
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)

	# link runtime statically
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	add_link_options(
		"-static-libgcc"
		"-static-libstdc++"
	)
endif()

if(NOT DEFINED OPENSSL_USE_STATIC_LIBS)
	set(OPENSSL_USE_STATIC_LIBS ON)
	set(OPENSSL_MSVC_STATIC_RT ON)
endif()

add_conan_lib(CONAN_PKG::openssl openssl/1.1.1h)

conan_check(REQUIRED)
conan_cmake_run(
	REQUIRES boost/1.73.0
	ARCH x86
	BASIC_SETUP CMAKE_TARGETS
	BUILD missing
	OPTIONS boost:zlib=False
)
set_target_properties(CONAN_PKG::boost PROPERTIES IMPORTED_GLOBAL TRUE)

add_subdirectory(libs/fmt)
mark_as_advanced(FMT_TEST FMT_INSTALL FMT_PEDANTIC FMT_DOC FMT_USE_CPP11 FMT_WERROR)

add_conan_lib(CONAN_PKG::tinydir tinydir/1.2@paulobrizolara/stable)
add_conan_lib(CONAN_PKG::yaml-cpp yaml-cpp/0.6.3)
add_subdirectory(src)