
set(base_SRCS 
	Common.cc
	Date.cc
	Timestamp.cc
	Logging.cc
	Thread.cc
	SysInfo.cc
	PerformanceCounter.cc
  )

add_library(mysdk_base ${base_SRCS})
target_link_libraries(mysdk_base pthread)

add_subdirectory(tests)