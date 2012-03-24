add_library(kabu_codec KaBuCodec.cc PacketBase.cc)
target_link_libraries(kabu_codec  mysdk_net)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=conversion")
##include_directories(${PROJECT_BINARY_DIR})
