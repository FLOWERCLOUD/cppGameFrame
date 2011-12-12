
add_executable(mysql_test mysql_test.cc)
target_link_libraries(mysql_test mysdk_mysql mysdk_base)

add_executable(bind_test TestDatabase.cc bind_test.cc)
target_link_libraries(bind_test mysdk_mysql mysdk_base)