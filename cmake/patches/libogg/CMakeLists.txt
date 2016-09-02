cmake_minimum_required(VERSION 2.8)

project(Ogg C)

set(LIBOGG_SRC
    src/bitwise.c
    src/framing.c
)
set(LIBOGG_HEADER
    include/ogg/ogg.h
    include/ogg/os_types.h
)

if(MSVC)
    ADD_DEFINITIONS(/DLIBOGG_EXPORTS /D_UNICODE /DUNICODE)
    LIST(APPEND LIBOGG_SRC win32/ogg.def)
endif(MSVC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(ogg SHARED ${LIBOGG_SRC} ${LIBOGG_HEADER})

if(UNIX AND NOT APPLE)
    # create "ogg/config_types.h"
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/ogg/config_types.h
        COMMAND ./configure --silent
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    add_custom_target(config_types SOURCES include/ogg/config_types.h)
    add_dependencies(ogg config_types)

    install(FILES include/ogg/config_types.h DESTINATION include/ogg)

endif(UNIX AND NOT APPLE)

install(TARGETS ogg
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${LIBOGG_HEADER} DESTINATION include/ogg)
