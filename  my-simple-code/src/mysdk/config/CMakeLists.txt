add_library(mysdk_config Config.cc)
target_link_libraries(mysdk_config mysdk_base)

add_subdirectory(tests)