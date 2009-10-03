# Common source files
SET(COMMON_SRCS
    Debug.cpp
    Message.cpp
    Player.cpp
    Properties.cpp
    graphics/DebugLayer.cpp
    graphics/Stage.cpp
    race/Block.cpp
    race/Bound.cpp
    race/Car.cpp
    race/Checkpoint.cpp
    race/Level.cpp
    race/RacePlayer.cpp
    race/TyreStripes.cpp
)

# Game client sources
SET(CLIENT_SRCS
    ${COMMON_SRCS}
    Application.cpp
    race/Race.cpp
    race/RaceScene.cpp
    race/RaceUI.cpp
    race/Viewport.cpp
    network/Client.cpp
    network/RaceClient.cpp
)

# Server sources
SET(SERVER_SRCS
    ${COMMON_SRCS}
    ServerApplication.cpp
    network/RaceServer.cpp
    network/Server.cpp
)

FIND_PACKAGE(ClanLib REQUIRED)
FIND_PACKAGE(Boost REQUIRED)

INCLUDE_DIRECTORIES(
    ${ClanLib_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    .
)

SET(CLIENT_LIBS ${LIBS}
    ${ClanLib_App_LIBRARY}
    ${ClanLib_Core_LIBRARY}
    ${ClanLib_Display_LIBRARY}
    ${ClanLib_GL_LIBRARY}
    ${ClanLib_Network_LIBRARY}
)

SET(SERVER_LIBS ${LIBS}
    ${ClanLib_App_LIBRARY}
    ${ClanLib_Core_LIBRARY}
    ${ClanLib_Network_LIBRARY}
    ${ClanLib_Display_LIBRARY} #TODO: try to remove this dependency
    ${ClanLib_GL_LIBRARY} #TODO: try to remove this dependency
)


# Game client configuration

add_executable(game ${CLIENT_SRCS})
TARGET_LINK_LIBRARIES(game ${CLIENT_LIBS})
SET_TARGET_PROPERTIES(game PROPERTIES LINK_FLAGS "-lz -lfontconfig -lX11 -lXxf86vm -lpng -ljpeg -lfreetype")
SET_TARGET_PROPERTIES(game PROPERTIES COMPILE_FLAGS "$ENV{CXXFLAGS}")

# Server configuration
add_executable(server ${SERVER_SRCS})
TARGET_LINK_LIBRARIES(server ${SERVER_LIBS})
SET_TARGET_PROPERTIES(server PROPERTIES LINK_FLAGS "-pthread -lfontconfig")
SET_TARGET_PROPERTIES(server PROPERTIES COMPILE_FLAGS "$ENV{CXXFLAGS}")

