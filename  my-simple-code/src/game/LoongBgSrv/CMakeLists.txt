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
  scene/Scene.cc
  skill/Buf.cc
  skill/BufHandler.cc
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
  JoinTimesMgr.cc
  Player.cc
  PlayerMgr.cc
  LoongBgSrv.cc
  main.cc
  )

include_directories("/usr/include/mysql/")
link_directories("/usr/lib/mysql/")

add_executable(LoongBgSrv ${LoongBgSrv_SRCS})
target_link_libraries(LoongBgSrv kabu_codec mysdk_exconfig mysdk_mysql mysdk_net curl mysqlclient )

add_subdirectory(tests)
