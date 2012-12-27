
set(sqltool_SRCS
	main.cc
	MySqlCon.cc
	../../ProtoImporter.cc
	../../Util.cc
  )

add_executable(sqltool ${sqltool_SRCS})
target_link_libraries(sqltool mysdk_exconfig   ${MYSQL_LIBRARY} ${PROTOBUF_LIBRARY} )
