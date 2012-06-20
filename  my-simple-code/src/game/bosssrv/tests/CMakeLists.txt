set(TestClient_SRCS
	Client.cc
	main.cc
  )
  
add_executable(TestClient ${TestClient_SRCS})
target_link_libraries(TestClient kabu_codec mysdk_net)

set(TestBoss_SRCS
        TestClient.cc
  )

add_executable(TestBoss ${TestBoss_SRCS})
target_link_libraries(TestBoss kabu_codec mysdk_net)
