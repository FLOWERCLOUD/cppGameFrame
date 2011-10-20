
add_executable(atomic_unittest Atomic_unittest.cc)

add_executable(date_unittest Date_unittest.cc)
target_link_libraries(date_unittest mysdk_base)

add_executable(timestamp_unittest Timestamp_unittest.cc)
target_link_libraries(timestamp_unittest mysdk_base)

add_executable(logging_unittest Logging_unittest.cc)
target_link_libraries(logging_unittest mysdk_base)


