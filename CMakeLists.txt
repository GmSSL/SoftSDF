cmake_minimum_required(VERSION 3.10)
project(softsdf C)

include(GNUInstallDirs)
include(CTest)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if(MSVC)
	add_compile_options(/W3)
else()
	add_compile_options(-Wall)
endif()

set(SOFTSDF_SRC softsdf.c)
set(SOFTSDFINIT_SRC softsdfinit.c)

set(GmSSL_ROOT_DIR "" CACHE PATH "Root directory of a GmSSL static installation")
set(GmSSL_INCLUDE_DIR "" CACHE PATH "Directory containing GmSSL headers")
set(GmSSL_LIBRARY "" CACHE FILEPATH "Path to the GmSSL static library")

find_path(GmSSL_INCLUDE_DIR
	NAMES gmssl/sm2.h
	HINTS ${GmSSL_ROOT_DIR}
	PATH_SUFFIXES include)

set(_softsdf_find_library_suffixes ${CMAKE_FIND_LIBRARY_SUFFIXES})
if(NOT WIN32)
	set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
endif()
find_library(GmSSL_LIBRARY
	NAMES gmssl libgmssl
	HINTS ${GmSSL_ROOT_DIR}
	PATH_SUFFIXES lib lib64)
set(CMAKE_FIND_LIBRARY_SUFFIXES ${_softsdf_find_library_suffixes})

find_program(GmSSL_GMSSL_EXECUTABLE
	NAMES gmssl gmssl.exe
	HINTS ${GmSSL_ROOT_DIR}
	PATH_SUFFIXES bin)

if(NOT GmSSL_INCLUDE_DIR)
	message(FATAL_ERROR "GmSSL headers not found. Set GmSSL_ROOT_DIR or GmSSL_INCLUDE_DIR.")
endif()
if(NOT GmSSL_LIBRARY)
	message(FATAL_ERROR "GmSSL static library not found. Set GmSSL_ROOT_DIR or GmSSL_LIBRARY.")
endif()

message(STATUS "GmSSL include dir: ${GmSSL_INCLUDE_DIR}")
message(STATUS "GmSSL library: ${GmSSL_LIBRARY}")
if(GmSSL_GMSSL_EXECUTABLE)
	message(STATUS "GmSSL executable: ${GmSSL_GMSSL_EXECUTABLE}")
endif()

add_library(softsdf SHARED ${SOFTSDF_SRC})
set_target_properties(softsdf PROPERTIES OUTPUT_NAME "softsdf")
target_include_directories(softsdf PRIVATE ${GmSSL_INCLUDE_DIR})
target_link_libraries(softsdf PRIVATE ${GmSSL_LIBRARY})

add_executable(softsdfinit ${SOFTSDFINIT_SRC})
target_include_directories(softsdfinit PRIVATE ${GmSSL_INCLUDE_DIR})
target_link_libraries(softsdfinit PRIVATE ${GmSSL_LIBRARY})

add_executable(softsdftest softsdftest.c)
target_include_directories(softsdftest PRIVATE ${GmSSL_INCLUDE_DIR})
target_link_libraries(softsdftest PRIVATE softsdf ${GmSSL_LIBRARY})

option(ENABLE_SM4_ECB "Enable SM4 ECB mode" ON)
if (ENABLE_SM4_ECB)
	add_definitions(-DENABLE_SM4_ECB)
endif()

option(ENABLE_SM4_OFB "Enable SM4 OFB mode" ON)
if (ENABLE_SM4_OFB)
	add_definitions(-DENABLE_SM4_OFB)
endif()

option(ENABLE_SM4_CFB "Enable SM4 CFB mode" ON)
if (ENABLE_SM4_CFB)
	add_definitions(-DENABLE_SM4_CFB)
endif()


if(APPLE)
	target_link_libraries(softsdf PRIVATE "-framework Security")
	target_link_libraries(softsdfinit PRIVATE "-framework Security")
	target_link_libraries(softsdftest PRIVATE "-framework Security")
endif()

if(APPLE)
    set_target_properties(softsdf PROPERTIES LINK_FLAGS "-Wl,-exported_symbols_list,${CMAKE_SOURCE_DIR}/softsdf.exp -Wl,-x")
elseif(UNIX)
    set_target_properties(softsdf PROPERTIES LINK_FLAGS "-Wl,--version-script=${CMAKE_SOURCE_DIR}/softsdf.lds -Wl,-x")
endif()

if(WIN32)
	target_link_libraries(softsdf PRIVATE Ws2_32 Advapi32)
	target_link_libraries(softsdfinit PRIVATE Ws2_32 Advapi32)
	target_link_libraries(softsdftest PRIVATE Ws2_32 Advapi32)
	set_target_properties(softsdf PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(BUILD_TESTING)
	add_test(NAME softsdftest COMMAND softsdftest)
	if(GmSSL_GMSSL_EXECUTABLE)
		add_test(NAME gmssl-sdftest
			COMMAND ${CMAKE_COMMAND}
				-DSOFTSDFINIT=$<TARGET_FILE:softsdfinit>
				-DSOFTSDF_LIBRARY=$<TARGET_FILE:softsdf>
				-DGMSSL_EXECUTABLE=${GmSSL_GMSSL_EXECUTABLE}
				-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/gmssl-sdftest
				-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/run-gmssl-sdftest.cmake)
	endif()
endif()

install(TARGETS softsdf softsdfinit
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES sdf.h sgd.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/softsdf)
