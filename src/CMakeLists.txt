include(AMXConfig)
include(AddSAMPPlugin)


set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/libs/sdk")
find_package(SAMPSDK REQUIRED)

set(DCC_CONFIG_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/config_headers")

configure_file(
	"../discord-connector.inc.in"
	"${CMAKE_BINARY_DIR}/artifact/pawno/include/discord-connector.inc"
	@ONLY
)

configure_file(
	"version.hpp.in"
	"${DCC_CONFIG_INCLUDE_DIR}/version.hpp"
	@ONLY
)

if(WIN32)
	set(CRASHHANDLER_CPP crashhandler_windows.cpp)
	set(FILECHANGEDETECTOR_CPP FileChangeDetectorWindows.cpp)
else()
	set(CRASHHANDLER_CPP crashhandler_unix.cpp)
	set(FILECHANGEDETECTOR_CPP FileChangeDetectorLinux.cpp)
endif()

add_subdirectory(logger/amx)

add_samp_plugin(discord-connector 
	${SAMPSDK_DIR}/amxplugin.cpp
	${SAMPSDK_DIR}/amxplugin2.cpp
	Bot.cpp
	Bot.hpp
	Channel.cpp
	Channel.hpp
	Error.hpp
	Guild.cpp
	Guild.hpp
	Logger.cpp
	Logger.hpp
	Message.cpp
	Message.hpp
	Network.cpp
	Network.hpp
	Singleton.hpp
	Http.cpp
	Http.hpp
	Callback.hpp
	PawnDispatcher.cpp
	PawnDispatcher.hpp
	Role.cpp
	Role.hpp
	User.cpp
	User.hpp
	WebSocket.cpp
	WebSocket.hpp
	Embed.cpp
	Embed.hpp
	Emoji.cpp
	Emoji.hpp
	main.cpp
	misc.hpp
	natives.cpp
	natives.hpp
	sdk.hpp
	types.hpp
	utils.hpp

	# log
	logger/Api.cpp
	logger/AmxDebugManager.cpp
	logger/AmxDebugManager.hpp
	logger/SampConfigReader.cpp
	logger/SampConfigReader.hpp
	logger/LogConfig.cpp
	logger/LogConfig.hpp
	logger/Logger.cpp
	logger/Logger.hpp
	logger/LogManager.cpp
	logger/LogManager.hpp
	logger/LogRotationManager.cpp
	logger/LogRotationManager.hpp
	logger/utils.cpp
	logger/utils.hpp
	logger/${CRASHHANDLER_CPP}
	logger/crashhandler.hpp
	logger/${FILECHANGEDETECTOR_CPP}
	logger/FileChangeDetector.hpp
	logger/LogLevel.hpp
	logger/ILogger.hpp
	logger/export.h

	plugin.def
)

if (UNIX)
	target_sources(discord-connector PUBLIC ${SAMPSDK_DIR}/amx/getch.c)
endif()

set_property(TARGET discord-connector PROPERTY CXX_STANDARD 14)
set_property(TARGET discord-connector PROPERTY CXX_STANDARD_REQUIRED ON)

if(MSVC)
set_target_properties(discord-connector
    PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/artifact>
	LIBRARY_OUTPUT_DIRECTORY  $<1:${CMAKE_BINARY_DIR}/artifact>
)
else(UNIX)
message(binary dir: ${CMAKE_BINARY_DIR})
set_target_properties(discord-connector
    PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/artifact
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/artifact
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/artifact
)
endif()


target_include_directories(discord-connector PRIVATE
	${OPENSSL_INCLUDE_DIR}
	${SAMPSDK_INCLUDE_DIR}
	${DCC_CONFIG_INCLUDE_DIR}
	${LOGCORE_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/libs/json
	${PROJECT_SOURCE_DIR}/libs/date/include
)

if(MSVC)
	target_compile_definitions(discord-connector PRIVATE
		BOOST_ALL_NO_LIB # disable auto-linking
		_CRT_SECURE_NO_WARNINGS
		NOMINMAX
		_WIN32_WINNT=0x0600
		_SCL_SECURE_NO_WARNINGS
		NOGDI
	)
	target_link_libraries(discord-connector Crypt32.lib)

endif()

target_compile_definitions(discord-connector PRIVATE ONLY_C_LOCALE) # make the date library use the C locale only
target_link_libraries(discord-connector amx fmt CONAN_PKG::boost CONAN_PKG::openssl CONAN_PKG::yaml-cpp
	CONAN_PKG::tinydir)

if(UNIX)
	if(NOT APPLE)
		target_link_libraries(discord-connector rt)
	endif()
	
	set_target_properties(discord-connector PROPERTIES
		INSTALL_RPATH "$ORIGIN/..:$ORIGIN/")
endif()

# increase warning levels
if (MSVC)
	target_compile_options(discord-connector PRIVATE
		/W4
		/WX
		/permissive-
	)

	# also enable multi-processor compilation
	target_compile_options(discord-connector PRIVATE
		/MP
	)
elseif(UNIX)
	target_compile_options(discord-connector PRIVATE
		-Wall
		-Wextra
		-pedantic
		-Wno-error
	)
endif()

if (MSVC)
	# WORKAROUND: Boost 1.70.0+ Beast has a bug that prevents using MSVC compiler optimizations
	# see https://github.com/boostorg/beast/issues/1582
	target_compile_options(discord-connector PUBLIC "$<$<CONFIG:Release>:/Od>")
endif()

add_custom_command(
    TARGET discord-connector POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
		${PROJECT_SOURCE_DIR}/LICENSE
		${CMAKE_BINARY_DIR}/artifact/LICENSE)