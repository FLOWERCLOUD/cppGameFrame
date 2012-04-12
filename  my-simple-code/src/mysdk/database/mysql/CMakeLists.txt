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

#We have our own custom modules that we use. This tells cmakes where to find them.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_modules/ )

find_package(MYSQL)
if (MYSQL_FOUND)
include_directories( ${MYSQL_INCLUDE_DIR} )
link_directories( ${MYSQL_LIBRARY} )
else (MYSQL_FOUND)
message(FATAL_ERROR, "MySQL NOT FOUND!")
endif (MYSQL_FOUND)

add_library(mysdk_mysql ${mysql_SRCS})
target_link_libraries(mysdk_mysql mysdk_net mysdk_base ${MYSQL_LIBRARY} -lz)

add_subdirectory(tests)
