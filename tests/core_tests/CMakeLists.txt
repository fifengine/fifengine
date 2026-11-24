#------------------------------------------------------------------------------
#                              FIFE C++ Tests
#------------------------------------------------------------------------------

message(STATUS "---------------------------------------------------------------")
message(STATUS "[ 🛠️ ] Building FIFE Tests")
message(STATUS "---------------------------------------------------------------")

find_package(Catch2 CONFIG REQUIRED)

if(NOT TARGET fife_lib)
  message(FATAL_ERROR "The fife tests require target 'fife_lib'. Configure with -DBUILD_LIBRARY=ON when enabling tests.")
endif()

set(
  FIFE_CORE_TEST_SOURCES
  test_atlas.cpp
  test_asset_resolver.cpp
  test_dat1.cpp
  test_dat2.cpp
  test_renderbackend_dummy.cpp
  test_enginesettings.cpp
  test_event_touch.cpp
  test_fife_math.cpp
  test_gui.cpp
  test_imagepool.cpp
  test_images.cpp
  test_key.cpp
  test_logger.cpp
  test_matrix.cpp
  test_point.cpp
  test_rect.cpp
  test_sharedptr.cpp
  test_text_layout.cpp
  test_utf.cpp
  test_vfs.cpp
  test_zip.cpp
  test_multicell_blocking.cpp
  test_multicell_pathfinding.cpp
  test_pathrenderer.cpp
  test_particle_attachment.cpp
  test_particle_emitter.cpp
  test_particle_manager.cpp
  test_renderernode.cpp
  test_font_types.cpp
  test_font_face.cpp
  test_font_instance.cpp
  test_font_definition_loader.cpp
  test_font_manager.cpp
  test_text_wrap.cpp
  test_textrenderpool.cpp
  test_window.cpp
  test_speechbubble.cpp
)

if(ENABLE_MOVIE)
  list(APPEND FIFE_CORE_TEST_SOURCES test_movie.cpp)
endif()

message(STATUS "All tests are linked into a single executable `all_tests`.")

add_executable(all_tests ${FIFE_CORE_TEST_SOURCES})

# Windows: make the test executable PerMonitorV2 DPI-aware. This only applies
# to the host .exe; the FIFE library is a DLL and cannot set process DPI
# awareness. No-op on other platforms.
fife_embed_windows_dpi_manifest(all_tests)

# Include directories for FIFE headers
target_include_directories(all_tests
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/fife
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

# Link the fife implementation - use object files for coverage on Linux
if(MSVC)
  # on MSVC link the public library target
  target_link_libraries(all_tests PRIVATE fife::fife)
else()
  # Use object files for coverage on Linux
  target_link_libraries(all_tests PRIVATE $<TARGET_OBJECTS:fife_lib>)
endif()

target_link_libraries(
  all_tests
    PRIVATE
      fife_options
      Catch2::Catch2WithMain
      PNG::PNG
      ZLIB::ZLIB
      ${PNG_LIBRARIES}
      Ogg::ogg
      Vorbis::vorbis
      Vorbis::vorbisfile
      Vorbis::vorbisenc
      OpenAL::OpenAL
      tinyxml2::tinyxml2
      $<$<BOOL:${ENABLE_LOGGING}>:spdlog::spdlog>
      utf8cpp::utf8cpp
      $<TARGET_NAME_IF_EXISTS:SDL3::SDL3main>
      $<TARGET_NAME_IF_EXISTS:SDL3::SDL3>
      $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image>,SDL3_image::SDL3_image,SDL3_image::SDL3_image-static>
      $<IF:$<TARGET_EXISTS:SDL3_ttf::SDL3_ttf>,SDL3_ttf::SDL3_ttf,SDL3_ttf::SDL3_ttf-static>
)

if(ENABLE_OPENGL)
  target_link_libraries(all_tests PRIVATE ${OPENGL_gl_LIBRARY} GLEW::GLEW)
endif()

if(ENABLE_FIFEGUI)
  target_link_libraries(all_tests PRIVATE fifechan::fifechan)
  if(TARGET fifechan::fifechan_sdl3)
    target_link_libraries(all_tests PRIVATE fifechan::fifechan_sdl3)
  endif()
  if(TARGET fifechan::fifechan_opengl)
    target_link_libraries(all_tests PRIVATE fifechan::fifechan_opengl)
  endif()
endif()

# Copy test resources to the build directory
set(RESOURCES_SRC_DIR "${CMAKE_SOURCE_DIR}/tests/data")
set(RESOURCES_DST_DIR "${CMAKE_CURRENT_BINARY_DIR}/tests/data")
set(LOGO_SRC_DIR "${CMAKE_SOURCE_DIR}/docs/logo")
set(LOGO_DST_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs/logo")

if(NOT EXISTS "${RESOURCES_DST_DIR}")
  file(MAKE_DIRECTORY "${RESOURCES_DST_DIR}")
endif()

file(GLOB_RECURSE RESOURCE_FILES "${RESOURCES_SRC_DIR}/*")
foreach(file IN LISTS RESOURCE_FILES)
  file(RELATIVE_PATH rel_path "${RESOURCES_SRC_DIR}" "${file}")
  get_filename_component(target_dir "${RESOURCES_DST_DIR}/${rel_path}" DIRECTORY)
  if(NOT EXISTS "${target_dir}")
    file(MAKE_DIRECTORY "${target_dir}")
  endif()
  if(NOT EXISTS "${RESOURCES_DST_DIR}/${rel_path}")
    file(COPY "${file}" DESTINATION "${target_dir}")
  endif()
endforeach()

file(MAKE_DIRECTORY "${LOGO_DST_DIR}")
if(EXISTS "${LOGO_SRC_DIR}/FIFE_small_c3.png" AND NOT EXISTS "${LOGO_DST_DIR}/FIFE_small_c3.png")
  file(COPY "${LOGO_SRC_DIR}/FIFE_small_c3.png" DESTINATION "${LOGO_DST_DIR}")
endif()

# All C++ tests live in one all_tests binary; these entries select subsets of it via Catch2 tags.
# all_tests_dummy runs only tests tagged [dummy], i.e. the headless tests that construct RenderBackendNull/Surface directly.
# It is a tag filter, not a backend switch: SDL/OpenGL tests keep their real backends and are not in this subset.
# CI uses it for the normal Linux jobs so they need no display server, window, or GL context; the GL job still runs all_tests under Xvfb.
# The dummy label also allows running the same subset locally with ctest -L dummy.
add_test(NAME all_tests COMMAND all_tests)
set_tests_properties(all_tests PROPERTIES LABELS core WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_test(NAME all_tests_dummy COMMAND all_tests "[dummy]")
set_tests_properties(all_tests_dummy PROPERTIES LABELS dummy WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

set_target_properties(all_tests PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)
