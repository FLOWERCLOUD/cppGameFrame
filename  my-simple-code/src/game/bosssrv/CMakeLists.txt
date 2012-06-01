
set(bosssrv_SRCS
  php/util/bytesbuffer.cpp
  php/htmlclient.cpp
  BossHurtMgr.cc
  BossSrv.cc
  main.cc
  )

add_definitions(-DHAVE_BACKTRACE)
##add_definitions(-DTEST)

#We have our own custom modules that we use. This tells cmakes where to find them.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_modules/ )

find_package(MYSQL)
if (MYSQL_FOUND)
include_directories( ${MYSQL_INCLUDE_DIR} )
link_directories( ${MYSQL_LIBRARY} )
else (MYSQL_FOUND)
message(FATAL_ERROR, "MySQL NOT FOUND!")
endif (MYSQL_FOUND)

add_executable(bosssrv ${bosssrv_SRCS})
target_link_libraries(bosssrv kabu_codec mysdk_exconfig mysdk_mysql mysdk_net curl)

add_subdirectory(tests)
