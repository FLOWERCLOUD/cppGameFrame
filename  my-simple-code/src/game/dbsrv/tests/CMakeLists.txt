set(TestDBClient_SRCS
	DBClient.cc
	TestDBSrv.cc
	../codec/codec.cc
	../msg.pb.cc
  )
  
add_executable(TestDBSrv ${TestDBClient_SRCS})
target_link_libraries(TestDBSrv mysdk_net protobuf)

