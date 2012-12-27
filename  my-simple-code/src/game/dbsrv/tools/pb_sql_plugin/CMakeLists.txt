
set(pb_sql_plugin_SRCS
	main.cc
	sql_generator.cc
	${CMAKE_SOURCE_DIR}/deps/protobuf/src/google/protobuf/compiler/code_generator.cc
	${CMAKE_SOURCE_DIR}/deps/protobuf/src/google/protobuf/compiler/command_line_interface.cc
	${CMAKE_SOURCE_DIR}/deps/protobuf/src/google/protobuf/compiler/importer.cc
	${CMAKE_SOURCE_DIR}/deps/protobuf/src/google/protobuf/compiler/subprocess.cc
	${CMAKE_SOURCE_DIR}/deps/protobuf/src/google/protobuf/compiler/zip_writer.cc
	${CMAKE_SOURCE_DIR}/deps/protobuf/src/google/protobuf/compiler/plugin.pb.cc
  )
  
#We have our own custom modules that we use. This tells cmakes where to find them.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_modules/ )

find_package(PROTOBUF)
if (PROTOBUF_FOUND)
include_directories( ${PROTOBUF_INCLUDE_DIR} )
link_directories( ${PROTOBUF_LIBRARY} )
message("PROTOBUF FOUND! include dir:${PROTOBUF_INCLUDE_DIR}, libray:${PROTOBUF_LIBRARY}")
else (PROTOBUF_FOUND)
message(FATAL_ERROR, "PROTOBUF NOT FOUND!")
endif (PROTOBUF_FOUND)

include_directories(${CMAKE_SOURCE_DIR}/deps/protobuf/src/)
add_executable(sqltools ${pb_sql_plugin_SRCS})
target_link_libraries(sqltools ${PROTOBUF_LIBRARY})

