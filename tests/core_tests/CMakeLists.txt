#------------------------------------------------------------------------------
#                           Find GoogleTest 
#------------------------------------------------------------------------------

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(BUILD_SHARED_LIBS OFF)

#------------------------------------------------------------------------------
# create_test:
# is a helper function to create a test executable, 
# linked it against gtest, then install and run it.
#------------------------------------------------------------------------------
function(create_test test)

  get_filename_component(testname ${test} NAME_WE)

  add_executable(${testname} ${test})

  target_link_libraries(${testname} fife ${GTEST_BOTH_LIBRARIES})

  add_test(NAME ${testname} COMMAND ${testname})

  #install(TARGETS testfoo DESTINATION bin)

  #add_custom_command(TARGET ${test}
  #  POST_BUILD
  #  COMMAND ${test}
  #  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  #  COMMENT "Running ${test}" VERBATIM
  #)
endfunction()

#------------------------------------------------------------------------------
#               Collect all test files and create test executables
#------------------------------------------------------------------------------


file(GLOB_RECURSE TESTS ${PROJECT_SOURCE_DIR}/tests/core_tests/*.cpp)

FOREACH(TEST ${TESTS})
    create_test(${TEST})
ENDFOREACH(TEST)
