#------------------------------------------------------------------------------
#                           Find GoogleTest 
#------------------------------------------------------------------------------

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(BUILD_SHARED_LIBS OFF)

#------------------------------------------------------------------------------
# create_test:
# is a helper function to create a test executable, 
# link it against fife and gtest, then install and run it.
#------------------------------------------------------------------------------
function(create_test test)

  get_filename_component(testname ${test} NAME_WE)

  add_executable(${testname} ${test})

  target_link_libraries(${testname} fife ${GTEST_BOTH_LIBRARIES})
  
  add_test(NAME ${testname} COMMAND ${testname})

  install(TARGETS ${testname} DESTINATION ${CMAKE_INSTALL_PREFIX}/tests)
  
  #add_dependencies(${TESTSUITE} ${testname})

endfunction()

#------------------------------------------------------------------------------
#               Collect all test files and create test binaries
#------------------------------------------------------------------------------

#file(GLOB_RECURSE TESTS ${PROJECT_SOURCE_DIR}/tests/core_tests/*.cpp)

set(TESTS
  ${PROJECT_SOURCE_DIR}/tests/core_tests/test_dat1.cpp
  ${PROJECT_SOURCE_DIR}/tests/core_tests/test_dat2.cpp
  #${PROJECT_SOURCE_DIR}/tests/core_tests/test_gui.cpp
  #${PROJECT_SOURCE_DIR}/tests/core_tests/test_imagepool.cpp
  #${PROJECT_SOURCE_DIR}/tests/core_tests/test_images.cpp
  #${PROJECT_SOURCE_DIR}/tests/core_tests/test_rect.cpp
  #${PROJECT_SOURCE_DIR}/tests/core_tests/test_sharedptr.cpp
  ${PROJECT_SOURCE_DIR}/tests/core_tests/test_vfs.cpp
  ${PROJECT_SOURCE_DIR}/tests/core_tests/test_zip.cpp
)

FOREACH(TEST ${TESTS})
  create_test(${TEST})
ENDFOREACH(TEST)

#------------------------------------------------------------------------------
#               Copy test resources into test binaries folder
#------------------------------------------------------------------------------

#add_custom_target(copy_test_resources ALL COMMENT "Target: Copy Resources")

#add_custom_command(TARGET copy_test_resources
#  POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E echo "== Copy Test Data"
#  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tests/data ${CMAKE_INSTALL_PREFIX}/tests
#)

file(COPY ${PROJECT_SOURCE_DIR}/tests/data DESTINATION ${CMAKE_INSTALL_PREFIX}/tests)
file(COPY ${PROJECT_SOURCE_DIR}/../fifengine-dependencies/lib/gtest.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/tests)
file(COPY ${PROJECT_SOURCE_DIR}/../fifengine-dependencies/lib/x86/zlib1.dll DESTINATION ${CMAKE_INSTALL_PREFIX}/tests/zlib.dll)
