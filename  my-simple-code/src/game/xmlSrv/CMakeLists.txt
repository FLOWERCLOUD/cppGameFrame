set(XmlSrv_SRCS
  XmlSrv.cc
  main.cc
  )


add_executable(XmlSrv ${XmlSrv_SRCS})
target_link_libraries(XmlSrv  mysdk_net)

