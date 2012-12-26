
##add_subdirectory(deps)

add_custom_command(OUTPUT  msg.pb.h msg.pb.cc
  COMMAND protoc
  ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/msg.proto -I${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS msg.proto)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=conversion -Wno-extra -Wno-shadow")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(dbsrv_SRCS
  cache/rcache.cc
  codec/codec.cc
  lua/LuaEngine.cc
  lua/LuaLog.cc
  lua/LuaMyLibs.cc
  lua/LuaMySql.cc
  lua/LuaPB.cc
  lua/LuaRedis.cc
  lua/LuaWK.cc
  mysql/Field.cc
  mysql/MySQLConnection.cc
  mysql/MySQLConnectionInfo.cc
  mysql/QueryResult.cc
  DBSrv.cc
  main.cc
  PBSql.cc
  ProtoImporter.cc
  Util.cc
  WorkerThread.cc
  WorkerThreadPool.cc
  WriterThread.cc
  WriterThreadPool.cc
  )

add_definitions(-DHAVE_BACKTRACE)
##add_definitions(-DTEST)

#We have our own custom modules that we use. This tells cmakes where to find them.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_modules/ )

find_package(MYSQL)
if (MYSQL_FOUND)
include_directories( ${MYSQL_INCLUDE_DIR} )
link_directories( ${MYSQL_LIBRARY} )
message("MySQL FOUND! include dir:${MYSQL_INCLUDE_DIR}, libray:${MYSQL_LIBRARY}")
else (MYSQL_FOUND)
message(FATAL_ERROR, "MySQL NOT FOUND!")
endif (MYSQL_FOUND)

find_package(LUA)
if (LUA_FOUND)
include_directories( ${LUA_INCLUDE_DIR} )
link_directories( ${LUA_LIBRARY} )
message("LUA FOUND! include dir:${LUA_INCLUDE_DIR}, libray:${LUA_LIBRARY}")
else (LUA_FOUND)
message(FATAL_ERROR, "LUA NOT FOUND!")
endif (LUA_FOUND)

find_package(REDIS)
if (REDIS_FOUND)
include_directories( ${REDIS_INCLUDE_DIR} )
link_directories( ${REDIS_LIBRARY} )
message("REDIS FOUND! include dir:${REDIS_INCLUDE_DIR}, libray:${REDIS_LIBRARY}")
else (REDIS_FOUND)
message(FATAL_ERROR, "REDIS NOT FOUND!")
endif (REDIS_FOUND)

find_package(PROTOBUF)
if (PROTOBUF_FOUND)
include_directories( ${PROTOBUF_INCLUDE_DIR} )
link_directories( ${PROTOBUF_LIBRARY} )
message("PROTOBUF FOUND! include dir:${PROTOBUF_INCLUDE_DIR}, libray:${PROTOBUF_LIBRARY}")
else (PROTOBUF_FOUND)
message(FATAL_ERROR, "PROTOBUF NOT FOUND!")
endif (PROTOBUF_FOUND)

add_library(msg_proto msg.pb.cc)
target_link_libraries(msg_proto ${PROTOBUF_LIBRARY})

add_executable(dbsrv ${dbsrv_SRCS})
target_link_libraries(dbsrv  msg_proto mysdk_exconfig  mysdk_net ${REDIS_LIBRARY} ${MYSQL_LIBRARY}  ${LUA_LIBRARY} )

add_custom_target(EXE ALL
  COMMAND cp ${CMAKE_BINARY_DIR}/bin/dbsrv ${CMAKE_SOURCE_DIR}/apps/dbsrv/bin
  COMMENT "install dbsrv"
  DEPENDS ${CMAKE_BINARY_DIR}/bin/dbsrv
)
  
add_subdirectory(tests)
add_subdirectory(lua/tests)

### 
set(dbclient_SRCS
  codec/codec.cc
  lua/LuaEngine.cc
  lua/LuaLog.cc
  lua/LuaPB.cc
  lua/LuaEngine.cc
  lua/LuaLog.cc
  lua/LuaPB.cc
  tests/DBLuaClient.cc
  tests/LuaClient.cc
  tests/main.cc
  ProtoImporter.cc
  Util.cc
  )
add_executable(dbclient ${dbclient_SRCS})
target_link_libraries(dbclient  msg_proto mysdk_exconfig  mysdk_net   ${LUA_LIBRARY} )

add_subdirectory(tools/pb_sql_plugin)