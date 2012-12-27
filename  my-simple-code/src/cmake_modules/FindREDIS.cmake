# - Find redis
# Find the native redis includes and library
#
#  REDIS_INCLUDE_DIR - where to find hiredis.h, etc.
#  REDIS_LIBRARIES   - List of libraries when using REDIS.
#  REDIS_FOUND       - True if REDIS found.

IF (REDIS_INCLUDE_DIR)
  # Already in cache, be silent
  SET(REDIS_FIND_QUIETLY TRUE)
ENDIF (REDIS_INCLUDE_DIR)

FIND_PATH(REDIS_INCLUDE_DIR hiredis.h
  /usr/local/include/
  /usr/include/
  /usr/local/include/hiredis
  /usr/include/hiredis
)

SET(REDIS_NAMES hiredis)
FIND_LIBRARY(REDIS_LIBRARY
  NAMES ${REDIS_NAMES}
  PATHS /usr/lib  /usr/local/lib /usr/lib/hiredis /usr/local/lib/hiredis
)

IF (REDIS_INCLUDE_DIR AND REDIS_LIBRARY)
  SET(REDIS_FOUND TRUE)
  SET( REDIS_LIBRARIES ${REDIS_LIBRARY} )
ELSE (REDIS_INCLUDE_DIR AND REDIS_LIBRARY)
  SET(REDIS_FOUND FALSE)
  SET( REDIS_LIBRARIES )
ENDIF (REDIS_INCLUDE_DIR AND REDIS_LIBRARY)

IF (REDIS_FOUND)
  IF (NOT REDIS_FIND_QUIETLY)
    MESSAGE(STATUS "Found REDIS: ${REDIS_LIBRARY}")
  ENDIF (NOT REDIS_FIND_QUIETLY)
ELSE (REDIS_FOUND)
  IF (REDIS_FIND_REQUIRED)
    MESSAGE(STATUS "Looked for REDIS libraries named ${REDIS_NAMES}.")
    MESSAGE(FATAL_ERROR "Could NOT find REDIS library")
  ENDIF (REDIS_FIND_REQUIRED)
ENDIF (REDIS_FOUND)

MARK_AS_ADVANCED(
  REDIS_LIBRARY
  REDIS_INCLUDE_DIR
  )

