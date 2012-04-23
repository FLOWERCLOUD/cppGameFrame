
set(LoongBgSrv_SRCS
  base/Base.cc
  base/BufBase.cc
  base/BufBaseMgr.cc
  base/PetBase.cc
  base/PetBaseMgr.cc
  base/ItemBase.cc
  base/ItemBaseMgr.cc
  base/SkillBase.cc
  base/SkillBaseMgr.cc
  config/ConfigMgr.h
  package/Item.cc
  package/Package.cc
  php/util/bytesbuffer.cpp
  php/htmlclient.cpp
  scene/DropItemMgr.cc
  scene/FlowerMgr.cc
  scene/Scene.cc
  skill/Buf.cc
  skill/BufHandler.cc
  skill/AddSpeedBuf.cc
  skill/MianYiBuf.cc
  skill/HunXuanBuf.cc
  skill/SkillHandler.cc
  skill/ItemHandler.cc
  skill/ZhouShangBuf.cc
  util/md5.cpp
  Battleground.cc
  BattlegroundMgr.cc
  BattlegroundState.cc
  BgBuilding.cc
  StateBattlegroundCountDown.cc
  StateBattlegroundExit.cc
  StateBattlegroundRun.cc
  StateBattlegroundStart.cc
  StateBattlegroundWait.cc
  BgPlayer.cc
  BgUnit.cc
  Hotel.cc
  JoinTimesMgr.cc
  Player.cc
  PlayerMgr.cc
  LoongBgSrv.cc
  main.cc
  )

add_definitions(-DHAVE_BACKTRACE)

#We have our own custom modules that we use. This tells cmakes where to find them.
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake_modules/ )

find_package(MYSQL)
if (MYSQL_FOUND)
include_directories( ${MYSQL_INCLUDE_DIR} )
link_directories( ${MYSQL_LIBRARY} )
else (MYSQL_FOUND)
message(FATAL_ERROR, "MySQL NOT FOUND!")
endif (MYSQL_FOUND)

add_executable(LoongBgSrv ${LoongBgSrv_SRCS})
target_link_libraries(LoongBgSrv kabu_codec mysdk_exconfig mysdk_mysql mysdk_net curl)

add_subdirectory(tests)
