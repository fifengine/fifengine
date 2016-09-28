cmake_minimum_required(VERSION 2.8.3)

project(TINYXML)

option(BUILD_SHARED_LIBS "Building shared libs. Default is ON" ON)
if(MSVC AND BUILD_SHARED_LIBS)
 set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_definitions(-DTIXML_USE_STL=1)

set(TINYXML_SRC
  tinystr.cpp
  tinyxml.cpp
  tinyxmlerror.cpp
  tinyxmlparser.cpp
)

set(TINYXML_HEADERS
  tinystr.h
  tinyxml.h
)

add_library(tinyxml ${TINYXML_SRC})

install(TARGETS tinyxml
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(FILES ${TINYXML_HEADERS} DESTINATION include)