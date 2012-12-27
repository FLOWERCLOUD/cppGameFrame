# - Find lua
# Find the native lua includes and library
#
#  LUA_INCLUDE_DIR - where to find lua.h, etc.
#  LUA_LIBRARIES   - List of libraries when using lua.
#  LUA_FOUND       - True if lua found.

IF (LUA_INCLUDE_DIR)
  # Already in cache, be silent
  SET(LUA_FIND_QUIETLY TRUE)
ENDIF (LUA_INCLUDE_DIR)

FIND_PATH(LUA_INCLUDE_DIR lua.h
  /usr/local/include/lua
  /usr/include
  /usr/include/lua
  /usr/local/lua/include
)

SET(LUA_NAMES lua)
FIND_LIBRARY(LUA_LIBRARY
  NAMES ${LUA_NAMES}
  PATHS /usr/lib /usr/lib/lua /usr/local/lib /usr/local/lua/lib /usr/local/lib/lua
)

IF (LUA_INCLUDE_DIR AND LUA_LIBRARY)
  SET(LUA_FOUND TRUE)
  SET( LUA_LIBRARIES ${LUA_LIBRARY} )
ELSE (LUA_INCLUDE_DIR AND LUA_LIBRARY)
  SET(LUA_FOUND FALSE)
  SET( LUA_LIBRARIES )
ENDIF (LUA_INCLUDE_DIR AND LUA_LIBRARY)

IF (LUA_FOUND)
  IF (NOT LUA_FIND_QUIETLY)
    MESSAGE(STATUS "Found LUA: ${LUA_LIBRARY}")
  ENDIF (NOT LUA_FIND_QUIETLY)
ELSE (LUA_FOUND)
  IF (LUA_FIND_REQUIRED)
    MESSAGE(STATUS "Looked for LUA libraries named ${LUA_NAMES}.")
    MESSAGE(FATAL_ERROR "Could NOT find LUA library")
  ENDIF (LUA_FIND_REQUIRED)
ENDIF (LUA_FOUND)

MARK_AS_ADVANCED(
  LUA_LIBRARY
  LUA_INCLUDE_DIR
  )

