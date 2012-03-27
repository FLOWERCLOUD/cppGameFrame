set(LoongBgSrv_SRCS
  base/BufBase.cc
  base/BufBaseMgr.cc
  base/PetBase.cc
  base/PetBaseMgr.cc
  base/SkillBase.cc
  base/SkillBaseMgr.cc
  scene/Scene.cc
  skill/Buf.cc
  skill/BufHandler.cc
  skill/HunXuanBuf.cc
  skill/SkillHandler.cc
  skill/ZhouShangBuf.cc
  util/md5.cpp
  Battleground.cc
  BattlegroundMgr.cc
  BattlegroundState.cc
  StateBattlegroundCountDown.cc
  StateBattlegroundExit.cc
  StateBattlegroundRun.cc
  StateBattlegroundStart.cc
  StateBattlegroundWait.cc
  BgPlayer.cc
  BgUnit.cc
  LoongBgSrv.cc
  main.cc
  )


add_executable(LoongBgSrv ${LoongBgSrv_SRCS})
target_link_libraries(LoongBgSrv kabu_codec mysdk_net)

add_subdirectory(tests)
