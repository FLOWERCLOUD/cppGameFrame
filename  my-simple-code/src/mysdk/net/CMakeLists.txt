set(net_SRCS
  Acceptor.cc
  Buffer.cc
  Connector.cc
  EventLoop.cc
  InetAddress.cc
  Poller.cc
  poller/DefaultPoller.cc
  poller/EPollPoller.cc
  Socket.cc
  SocketsOps.cc
  Session.cc
  TcpClient.cc
  TcpConnection.cc
  TcpServer.cc
  Timer.cc
  TimerQueue.cc
  )

add_library(mysdk_net ${net_SRCS})
target_link_libraries(mysdk_net mysdk_base)

add_subdirectory(tests)
