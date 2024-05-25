cmake_minimum_required(VERSION 3.10)
project(softsdf C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall")

set(SOFTSDF_SRC softsdf.c)
set(SOFTSDFINIT_SRC softsdfinit.c)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_library(softsdf SHARED ${SOFTSDF_SRC})
set_target_properties(softsdf PROPERTIES OUTPUT_NAME "softsdf")

target_link_libraries(softsdf /usr/local/lib/libgmssl.a)

if(APPLE)
    target_link_libraries(softsdf "-framework Security")
endif()

if(APPLE)
    set_target_properties(softsdf PROPERTIES LINK_FLAGS "-Wl,-exported_symbols_list,${CMAKE_SOURCE_DIR}/softsdf.exp -Wl,-x")
else()
    set_target_properties(softsdf PROPERTIES LINK_FLAGS "-Wl,--version-script=${CMAKE_SOURCE_DIR}/softsdf.exp")
endif()

add_executable(softsdfinit ${SOFTSDFINIT_SRC})
target_link_libraries(softsdfinit gmssl)

add_executable(softsdftest softsdftest.c)
target_link_libraries(softsdftest softsdf gmssl)
