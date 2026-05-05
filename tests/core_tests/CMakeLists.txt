#------------------------------------------------------------------------------
#                              Core C++ Tests
#------------------------------------------------------------------------------

find_package(Catch2 CONFIG REQUIRED)

if(NOT TARGET fife)
  message(FATAL_ERROR "Core tests require target 'fife'. Configure with -DBUILD_LIBRARY=ON when enabling tests.")
endif()

option(CORE_TESTS_GUI_EXPERIMENTAL "Build GUI/imagepool core tests (currently unported to current engine APIs)" OFF)
option(CORE_TESTS_ZIP              "Build zip core test (currently unstable)" OFF)

set(
  FIFE_CORE_TEST_SOURCES
  test_dat1.cpp
  test_dat2.cpp
  test_rect.cpp
  test_sharedptr.cpp
  test_utf.cpp
  test_vfs.cpp
)

if(CORE_TESTS_ZIP)
  list(APPEND FIFE_CORE_TEST_SOURCES test_zip.cpp)
endif()

if(CORE_TESTS_GUI_EXPERIMENTAL)
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
        tinyxml2::tinyxml2
        $<TARGET_NAME_IF_EXISTS:SDL3::SDL3main>
        $<TARGET_NAME_IF_EXISTS:SDL3::SDL3>
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image>,SDL3_image::SDL3_image,SDL3_image::SDL3_image-static>
        $<IF:$<TARGET_EXISTS:SDL3_ttf::SDL3_ttf>,SDL3_ttf::SDL3_ttf,SDL3_ttf::SDL3_ttf-static>
  )

  if(ENABLE_OPENGL)
    target_link_libraries(${test_name} PRIVATE ${OPENGL_gl_LIBRARY} GLEW::GLEW)
  endif()

  if(ENABLE_FIFEGUI)
    target_link_libraries(${test_name} PRIVATE ${FIFECHAN_LIBRARIES})
    if(TARGET fifechan::fifechan_opengl)
      target_link_libraries(${test_name} PRIVATE fifechan::fifechan_opengl)
    endif()
  endif(ENABLE_FIFEGUI)

  add_test(NAME ${test_name} COMMAND ${test_name})
  set_tests_properties(${test_name} PROPERTIES LABELS core WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endforeach()
