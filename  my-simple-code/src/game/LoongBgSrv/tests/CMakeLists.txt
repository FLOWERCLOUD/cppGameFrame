set(TestLoongBgClient_SRCS
	../util/md5.cpp
	TestLoongBgSrv.cc
  )
  
add_executable(TestLoongBgClient ${TestLoongBgClient_SRCS})
target_link_libraries(TestLoongBgClient kabu_codec mysdk_net)

set(LoongBgSrvBenchmark_SRCS
	../util/md5.cpp
	BgClientData.cc
	BgClientDataMgr.cc
	LoongBgClient.cc
	LoongBgSrvBenchmark.cc
  )
  
add_executable(LoongBgSrvBenchmark ${LoongBgSrvBenchmark_SRCS})
target_link_libraries(LoongBgSrvBenchmark kabu_codec mysdk_net)

add_executable(TestBenchmark TestBen.cc)
target_link_libraries(TestBenchmark   mysdk_net)
