cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
project(samp-log-core VERSION 0.4)

if (NOT LOGCORE_VERSION) # allow version override
	set(LOGCORE_VERSION ${PROJECT_VERSION})
endif()

add_subdirectory(src)