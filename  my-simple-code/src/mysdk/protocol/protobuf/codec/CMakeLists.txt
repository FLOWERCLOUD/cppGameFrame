add_library(protobuf_codec codec.cc)
target_link_libraries(protobuf_codec protobuf mysdk_net)

add_subdirectory(tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=conversion")
##include_directories(${PROJECT_BINARY_DIR})
