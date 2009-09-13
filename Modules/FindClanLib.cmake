# - Find ClanLib
# ClanLib is a cross platform SDK geared toward making games. It is
# available from http://clanlib.org.
#
# The following are defined by this module:
#  ClanLib_FOUND - TRUE if ClanLib was found
#  ClanLib_INCLUDE_DIRS - Directory containing the ClanLib headers
#  ClanLib_LIBRARIES - If invoked via FIND_PACKAGE(ClanLib COMPONENTS ...),
#                      will only contain the libraries matching each component.
#                      otherwise, it will contain all ClanLib libraries found.
# For the components Core, App, Display, GL, GUI, GUIStyleSilver, MikMod,
# Network, SDL, Signals, Sound and Vorbis, the following variables are set:
#  ClanLib_${COMPONENT}_LIBRARY - Full path to the component's library.
IF(ClanLib_INCLUDE_DIRS)
  SET(ClanLib_FIND_QUIETLY TRUE)
ENDIF(ClanLib_INCLUDE_DIRS)

IF(NOT ClanLib_FIND_COMPONENTS)
  SET(ClanLib_FIND_COMPONENTS
    App
    Display
    GL
    GUI
    GUIStyleSilver
    MikMod
    Network
    SDL
    Signals
    Sound
    Vorbis)
ENDIF(NOT ClanLib_FIND_COMPONENTS)

MACRO(ClanLib_MSG MSG)
  IF(NOT ClanLib_FIND_QUIETLY)
    MESSAGE(STATUS ${MSG})
  ENDIF(NOT ClanLib_FIND_QUIETLY)
ENDMACRO(ClanLib_MSG)

MACRO(ClanLib_ERR MSG)
  IF(ClanLib_FIND_REQUIRED)
    MESSAGE(SEND_ERROR ${MSG})
  ELSE(ClanLib_FIND_REQUIRED)
    ClanLib_MSG(${MSG})
  ENDIF(ClanLib_FIND_REQUIRED)
ENDMACRO(ClanLib_ERR)

MACRO(ClanLib_FIND_COMPONENT COMPONENT)
  ClanLib_MSG("Checking for Clan${COMPONENT}")
  FIND_LIBRARY(ClanLib_${COMPONENT}_LIBRARY clan${COMPONENT}
    ${CLANLIB_ROOT_DIR}/lib /lib /usr/lib /usr/local/lib
    DOC "Library name for clan${COMPONENT}.")
  IF(ClanLib_${COMPONENT}_LIBRARY)
    SET(ClanLib_${COMPONENT}_FOUND TRUE)
    ClanLib_MSG("Checking for Clan${COMPONENT} -- ${ClanLib_${COMPONENT}_LIBRARY}")
  ELSE(ClanLib_${COMPONENT}_LIBRARY)
    SET(ClanLib_${COMPONENT}_FOUND FALSE)
    IF(ClanLib_FIND_REQUIRED_${COMPONENT})
      ClanLib_ERR("Checking for Clan${COMPONENT} -- not found")
    ELSE(ClanLib_FIND_REQUIRED_${COMPONENT})
      ClanLib_MSG("Checking for Clan${COMPONENT} -- not found")
    ENDIF(ClanLib_FIND_REQUIRED_${COMPONENT})
  ENDIF(ClanLib_${COMPONENT}_LIBRARY)
ENDMACRO(ClanLib_FIND_COMPONENT)

ClanLib_MSG("Checking for ClanLib")

FIND_PATH(ClanLib_INCLUDE_DIRS ClanLib/core.h
  ${ClanLib_ROOT_DIR}/include ${ClanLib_ROOT_DIR}/include/ClanLib-2.0
  /usr/local/include          /usr/local/include/ClanLib-2.0
  /usr/include                /usr/include/ClanLib-2.0
  DOC "Where to find the ClanLib includes.")
IF(ClanLib_INCLUDE_DIRS)
  ClanLib_MSG("Checking for ClanLib -- headers")
ELSE(ClanLib_INCLUDE_DIRS)
  ClanLib_ERR("Checking for ClanLib -- headers not found")
ENDIF(ClanLib_INCLUDE_DIRS)

ClanLib_FIND_COMPONENT(Core)
IF(ClanLib_INCLUDE_DIRS AND ClanLib_Core_LIBRARY)
  SET(ClanLib_FOUND TRUE)
  SET(ClanLib_LIBRARIES ${ClanLib_Core_LIBRARY})
ELSE(ClanLib_INCLUDE_DIRS AND ClanLib_Core_LIBRARY)
  SET(ClanLib_FOUND FALSE)
ENDIF(ClanLib_INCLUDE_DIRS AND ClanLib_Core_LIBRARY)

ClanLib_MSG("Checking for other ClanLib components")
FOREACH(COMPONENT ${ClanLib_FIND_COMPONENTS})
  ClanLib_FIND_COMPONENT(${COMPONENT})
  IF(ClanLib_${COMPONENT}_LIBRARY)
    LIST(APPEND ClanLib_LIBRARIES ${ClanLib_${COMPONENT}_LIBRARY})
  ENDIF(ClanLib_${COMPONENT}_LIBRARY)
ENDFOREACH(COMPONENT)

MARK_AS_ADVANCED(
  ClanLib_INCLUDE_DIRS
  ClanLib_App_LIBRARY
  ClanLib_Core_LIBRARY
  ClanLib_Display_LIBRARY
  ClanLib_GL_LIBRARY
  ClanLib_GUI_LIBRARY
  ClanLib_GUIStyleSilver_LIBRARY
  ClanLib_MikMod_LIBRARY
  ClanLib_Network_LIBRARY
  ClanLib_SDL_LIBRARY
  ClanLib_Signals_LIBRARY
  ClanLib_Sound_LIBRARY
  ClanLib_Vorbis_LIBRARY
)
