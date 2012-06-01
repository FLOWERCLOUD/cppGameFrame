set(TestClient_SRCS
	Client.cc
	main.cc
  )
  
add_executable(TestClient ${TestClient_SRCS})
target_link_libraries(TestClient kabu_codec mysdk_net)
