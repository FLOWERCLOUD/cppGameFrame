set(LoongBgSrv_SRCS

  base/BufBase.cc
  base/BufBaseMgr.cc
  base/PetBase.cc
  base/PetBaseMgr.cc
  base/ItemBase.cc
  base/ItemBaseMgr.cc
  base/SkillBase.cc
  base/SkillBaseMgr.cc
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


add_executable(LoongBgSrv ${LoongBgSrv_SRCS})
target_link_libraries(LoongBgSrv kabu_codec mysdk_net curl)

add_subdirectory(tests)
