set(TestDBClient_SRCS
	linenoise.cc
	ClientThread.cc
	DBClient.cc
	##TestDBSrv.cc
	TestTerminal.cc
	../codec/codec.cc
	../ProtoImporter.cc
	../Util.cc
	../msg.pb.cc
  )
  
add_executable(TestTerminal ${TestDBClient_SRCS})
target_link_libraries(TestTerminal mysdk_net protobuf mysdk_exconfig)

