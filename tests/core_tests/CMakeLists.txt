#------------------------------------------------------------------------------
#                              Core C++ Tests
#------------------------------------------------------------------------------

find_package(Catch2 CONFIG REQUIRED)

if(NOT TARGET fife)
  message(FATAL_ERROR "Core tests require target 'fife'. Configure with -Dbuild-library=ON when enabling tests.")
endif()

option(core-tests-gui-experimental "Build GUI/imagepool core tests (currently unported to current engine APIs)" OFF)
option(core-tests-zip "Build zip core test (currently unstable)" OFF)

set(
  FIFE_CORE_TEST_SOURCES
  test_dat1.cpp
  test_dat2.cpp
  test_rect.cpp
  test_sharedptr.cpp
  test_vfs.cpp
)

if(core-tests-zip)
  list(APPEND FIFE_CORE_TEST_SOURCES test_zip.cpp)
endif()

if(core-tests-gui-experimental)
  list(APPEND FIFE_CORE_TEST_SOURCES
    test_gui.cpp
    test_imagepool.cpp
    test_images.cpp
  )
endif()

foreach(test_source ${FIFE_CORE_TEST_SOURCES})
  get_filename_component(test_name ${test_source} NAME_WE)

  add_executable(${test_name} ${test_source})
  target_link_options(${test_name} PRIVATE -Wl,--no-as-needed)
  target_link_libraries(
    ${test_name}
      PRIVATE
        Catch2::Catch2WithMain
        fife
        ZLIB::ZLIB
        ${PNG_LIBRARIES}
        Ogg::ogg
        Vorbis::vorbis
        Vorbis::vorbisfile
        Vorbis::vorbisenc
        OpenAL::OpenAL
        unofficial-tinyxml::unofficial-tinyxml
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
          $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
  )

  if(opengl)
    target_link_libraries(${test_name} PRIVATE ${OPENGL_gl_LIBRARY} GLEW::GLEW)
  endif(opengl)

  if(fifechan)
    target_link_libraries(${test_name} PRIVATE ${FIFECHAN_LIBRARIES})
    if(TARGET Fifechan::opengl)
      target_link_libraries(${test_name} PRIVATE Fifechan::opengl)
    elseif(DEFINED FIFECHAN_opengl_LIBRARY AND EXISTS "${FIFECHAN_opengl_LIBRARY}")
      target_link_libraries(${test_name} PRIVATE "${FIFECHAN_opengl_LIBRARY}")
    endif()
  endif(fifechan)

  if(cegui)
    target_link_libraries(${test_name} PRIVATE ${CEGUI_LIBRARIES} ${CEGUI_OpenGLRenderer_LIBRARIES})
  endif(cegui)

  add_test(NAME ${test_name} COMMAND ${test_name})
  set_tests_properties(${test_name} PROPERTIES LABELS core WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endforeach()
