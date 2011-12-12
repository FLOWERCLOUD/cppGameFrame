set(mysql_SRCS
	BindResultSet.cc
	##DatabaseWorkerThread.cc
	DBWorkerThread.cc
	DatabaseWorkerPool.cc
	Field.cc
	MySQLBinds.cc
	MySQLConnection.cc
	MySQLStmt.cc
	QueryResult.cc
	Transaction.cc
  )

include_directories("/usr/include/mysql/")
link_directories("/usr/lib/mysql/")

add_library(mysdk_mysql ${mysql_SRCS})
target_link_libraries(mysdk_mysql mysdk_net mysdk_base mysqlclient -lz)



add_subdirectory(tests)