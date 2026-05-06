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

message(STATUS "All tests are linked into a single executable `all_tests`.")

add_executable(all_tests ${FIFE_CORE_TEST_SOURCES})

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

target_link_options(all_tests PRIVATE -Wl,--no-as-needed)
target_link_libraries(
  all_tests
    PRIVATE
      Catch2::Catch2WithMain
      ZLIB::ZLIB
      ${PNG_LIBRARIES}
      Ogg::ogg
      Vorbis::vorbis
      Vorbis::vorbisfile
      Vorbis::vorbisenc
      OpenAL::OpenAL
      tinyxml2::tinyxml2
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

add_test(NAME all_tests COMMAND all_tests)
set_tests_properties(all_tests PROPERTIES LABELS core WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

set_target_properties(all_tests PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)
