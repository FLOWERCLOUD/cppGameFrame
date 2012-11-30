
set(dbsrv_SRCS
  cache/rcache.cc
  codec/codec.cc
  mysql/Field.cc
  mysql/MySQLConnection.cc
  mysql/MySQLConnectionInfo.cc
  mysql/QueryResult.cc
  DBSrv.cc
  main.cc
  msg.pb.cc
  WorkerThread.cc
  WorkerThreadPool.cc
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

add_executable(dbsrv ${dbsrv_SRCS})
target_link_libraries(dbsrv mysdk_exconfig  mysdk_net hiredis ${MYSQL_LIBRARY}  protobuf )

add_subdirectory(tests)

