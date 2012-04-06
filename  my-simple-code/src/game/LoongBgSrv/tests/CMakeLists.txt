set(TestLoongBgClient_SRCS
	../util/md5.cpp
	TestLoongBgSrv.cc
  )
  
add_executable(TestLoongBgClient ${TestLoongBgClient_SRCS})
target_link_libraries(TestLoongBgClient kabu_codec mysdk_net)

