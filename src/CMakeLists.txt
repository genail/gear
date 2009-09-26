# Common source files
SET(COMMON_SRCS
    Debug.cpp
    Message.cpp
    Player.cpp
    Properties.cpp
)

# Game client sources
SET(CLIENT_SRCS
    ${COMMON_SRCS}
    Application.cpp
    graphics/DebugLayer.cpp
    graphics/Stage.cpp
    race/Block.cpp
    race/Bound.cpp
    race/Car.cpp
    race/Checkpoint.cpp
    race/Level.cpp
    race/Race.cpp
    race/RacePlayer.cpp
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
    network/Server.cpp
)

FIND_PACKAGE(ClanLib REQUIRED)

INCLUDE_DIRECTORIES(
    ${ClanLib_INCLUDE_DIRS}
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
)


# Game client configuration

add_executable(game ${CLIENT_SRCS})
TARGET_LINK_LIBRARIES(game ${CLIENT_LIBS})
SET_TARGET_PROPERTIES(game PROPERTIES LINK_FLAGS "-lz -lfontconfig -lX11 -lXxf86vm -lpng -ljpeg -lfreetype")

# Server configuration
add_executable(server ${SERVER_SRCS})
TARGET_LINK_LIBRARIES(server ${SERVER_LIBS})
SET_TARGET_PROPERTIES(server PROPERTIES LINK_FLAGS "-pthread")
