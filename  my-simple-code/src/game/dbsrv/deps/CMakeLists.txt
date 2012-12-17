
#We have our own custom modules that we use. This tells cmakes where to find them.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_modules/ )

find_package(MYSQL)
if (!MYSQL_FOUND)
message("MySQL NOT FOUND!")
add_custom_target(MYSQLCLIENT ALL 
  COMMAND make  -C ${CMAKE_SOURCE_DIR}/deps/mysql
  COMMAND make install -C ${CMAKE_SOURCE_DIR}/deps/mysql 
  COMMENT "make && make install mysql"
)
endif (!MYSQL_FOUND)

find_package(LUA)
if (!LUA_FOUND)
add_custom_target(LUA ALL 
  COMMAND make linux -C ${CMAKE_SOURCE_DIR}/deps/lua
  COMMAND make install -C ${CMAKE_SOURCE_DIR}/deps/lua
  COMMENT "make && make install lua"
)
endif (!LUA_FOUND)

find_package(REDIS)
if (!REDIS)
add_custom_target(HIREDIS ALL 
  COMMAND make -C ${CMAKE_SOURCE_DIR}/deps/hiredis
  COMMAND make install -C ${CMAKE_SOURCE_DIR}/deps/hiredis
  COMMENT "make && make install hiredis"
)
endif (!REDIS)

find_package(PROTOBUF)
if (!PROTOBUF)
add_custom_target(PROTOBUF ALL 
  COMMAND make  -C ${CMAKE_SOURCE_DIR}/deps/protobuf
  COMMAND make install -C ${CMAKE_SOURCE_DIR}/deps/protobuf
  COMMENT "make && make install protobuf"
)
endif (!PROTOBUF)




