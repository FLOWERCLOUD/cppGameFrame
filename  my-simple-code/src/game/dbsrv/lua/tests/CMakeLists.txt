set(Testlua_SRCS
	TestLuaEngine.cc
	../LuaEngine.cc
  )
  
##add_executable(testlua ${Testlua_SRCS})
##target_link_libraries(testlua lua)

set(Testluapb_SRCS
	TestLuaPB.cc
	../LuaEngine.cc
	../LuaPB.cc
  )
  
add_executable(testluapb ${Testluapb_SRCS})
target_link_libraries(testluapb lua protobuf)

