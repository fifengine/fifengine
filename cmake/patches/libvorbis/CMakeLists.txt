cmake_minimum_required(VERSION 2.8)

project(LIBVORBIS C)

message("CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
message("OGG_LIBRARY=${OGG_LIBRARY}")
message("OGG_INCLUDE_DIR=${OGG_INCLUDE_DIR}")

#find_package(Ogg REQUIRED)

include_directories(
    ${OGG_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/lib
)

set(LIBVORBIS_SOURCE
    lib/analysis.c
    #lib/barkmel.c  # unnecessary: bark scale utility
    lib/bitrate.c
    lib/block.c
    lib/codebook.c
    lib/envelope.c
    lib/floor0.c
    lib/floor1.c
    lib/info.c
    lib/lookup.c
    lib/lpc.c
    lib/lsp.c
    lib/mapping0.c
    lib/mdct.c
    lib/psy.c
    #lib/psytune.c   # dead code
    lib/registry.c
    lib/res0.c
    lib/sharedbook.c
    lib/smallft.c
    lib/synthesis.c
    #lib/tone.c      # error: 'M_PI': undeclared identifier
    lib/vorbisenc.c
    lib/window.c
)

SET(libvorbis_HEADER
    lib/backends.h
    lib/bitrate.h
    lib/codebook.h
    lib/codec_internal.h
    lib/envelope.h
    lib/modes/floor_all.h
    lib/books/floor/floor_books.h
    lib/highlevel.h
    lib/lookup.h
    lib/lookup_data.h
    lib/lpc.h
    lib/lsp.h
    lib/masking.h
    lib/mdct.h
    lib/misc.h
    lib/os.h
    lib/psy.h
    lib/modes/psych_11.h
    lib/modes/psych_16.h
    lib/modes/psych_44.h
    lib/modes/psych_8.h
    lib/registry.h
    lib/books/coupled/res_books_stereo.h
    lib/books/uncoupled/res_books_uncoupled.h
    lib/modes/residue_16.h
    lib/modes/residue_44.h
    lib/modes/residue_44u.h
    lib/modes/residue_8.h
    lib/scales.h
    lib/modes/setup_11.h
    lib/modes/setup_16.h
    lib/modes/setup_22.h
    lib/modes/setup_32.h
    lib/modes/setup_44.h
    lib/modes/setup_44u.h
    lib/modes/setup_8.h
    lib/modes/setup_X.h
    lib/smallft.h
    lib/window.h
)

if(WIN32 AND NOT MINGW)
    set(vorbis_defs     win32/vorbis.def)
    set(vorbisfile_defs win32/vorbisfile.def)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(vorbis             SHARED ${LIBVORBIS_SOURCE} ${LIBVORBIS_HEADER} ${vorbis_defs})
add_library(vorbisfile         SHARED lib/vorbisfile.c ${vorbisfile_defs})
add_library(vorbisfile_static  STATIC lib/vorbisfile.c ${vorbisfile_defs})

if(UNIX)
    set(PLATFORM_LIBS m)
endif(UNIX)

target_link_libraries(vorbis            ${OGG_LIBRARY} ${PLATFORM_LIBS})
target_link_libraries(vorbisfile vorbis ${OGG_LIBRARY})

if(NOT WIN32)
    set_target_properties(vorbisfile_static PROPERTIES OUTPUT_NAME vorbisfile)
endif(NOT WIN32)

install(TARGETS vorbis vorbisfile
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/vorbis DESTINATION include FILES_MATCHING PATTERN "*.h")