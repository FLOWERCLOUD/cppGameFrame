add_library(mysdk_exconfig Config.cpp)
target_link_libraries(mysdk_exconfig mysdk_base)

set(testconf_SRCS
	test.cc
  )
  
add_executable(testconf ${testconf_SRCS})
target_link_libraries(testconf   mysdk_exconfig  mysdk_base )
