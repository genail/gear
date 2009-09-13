FIND_PATH(ClanLib-2.0_INCLUDE_DIR ClanLib
    PATH_SUFFIXES include/ClanLib-2.0
    PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include
    /opt/local/include
    /opt/csw/include 
    /opt/include
)

#MESSAGE(ClanLib-2.0_INCLUDE_DIR is ${ClanLib-2.0_INCLUDE_DIR})

FIND_LIBRARY(ClanLib-2.0_LIBRARY
    NAMES clanCore
    PATH_SUFFIXES lib64 lib
    PATHS
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt
)

#MESSAGE("ClanLib-2.0_LIBRARY is ${ClanLib-2.0_LIBRARY}")

#SET(ClabLib-2.0_FOUND true)
#SET(ClanLib-2.0_PROCESS_INCLUDES ClanLib-2.0_INCLUDE_DIR)
#SET(ClanLib-2.0_PROCESS_LIBS ClanLib-2.0_LIBRARY)
#libfind_process(ClanLib-2.0)
