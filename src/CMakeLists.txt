# Common source files
SET(COMMON_SRCS
    Player.cpp
    Properties.cpp
    network/CarState.cpp
    network/ClientInfo.cpp
    network/GameState.cpp
    network/Goodbye.cpp
    network/PlayerJoined.cpp
    race/Block.cpp
    race/Bound.cpp
    race/Car.cpp
    race/Checkpoint.cpp
    race/Level.cpp
    race/ScoreTable.cpp
)

# Game client sources
SET(CLIENT_SRCS
    ${COMMON_SRCS}
    Application.cpp
    Game.cpp
    controllers/LoadingController.cpp
    controllers/MainMenuController.cpp
    graphics/DebugLayer.cpp
    graphics/GameWindow.cpp
    graphics/SpeedMeter.cpp
    graphics/Stage.cpp
    gui/MainMenuScene.cpp
    gui/LoadingScene.cpp
    gui/SceneContainer.cpp
#    race/Race.cpp
    race/RaceScene.cpp
    race/RaceUI.cpp
    race/TyreStripes.cpp
    race/Viewport.cpp
    network/Client.cpp
)

# Server sources
SET(SERVER_SRCS
    ${COMMON_SRCS}
    ServerApplication.cpp
    ServerConfiguration.cpp
    network/Server.cpp
)

FIND_PACKAGE(ClanLib-2.1 REQUIRED)
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
    ${ClanLib_GL1_LIBRARY}
    ${ClanLib_Network_LIBRARY}
    ${ClanLib_GUI_LIBRARY}
)

SET(SERVER_LIBS ${LIBS}
    ${ClanLib_App_LIBRARY}
    ${ClanLib_Core_LIBRARY}
    ${ClanLib_Network_LIBRARY}
)

SET(GUI_LIBS ${LIBS}
    ${ClanLib_App_LIBRARY}
    ${ClanLib_Core_LIBRARY}
    ${ClanLib_GUI_LIBRARY}
    ${ClanLib_Display_LIBRARY}
    ${ClanLib_GL_LIBRARY}

)


# Game client configuration

add_executable(game ${CLIENT_SRCS})
TARGET_LINK_LIBRARIES(game ${CLIENT_LIBS})
SET_TARGET_PROPERTIES(game PROPERTIES LINK_FLAGS "-lz -lfontconfig -lX11 -lXxf86vm -lpng -ljpeg -lfreetype")
SET_TARGET_PROPERTIES(game PROPERTIES COMPILE_FLAGS "-Wall -DCLIENT -DGL1 $ENV{CXXFLAGS}")

# Server configuration
add_executable(server ${SERVER_SRCS})
TARGET_LINK_LIBRARIES(server ${SERVER_LIBS})
SET_TARGET_PROPERTIES(server PROPERTIES LINK_FLAGS "-pthread -lfontconfig")
SET_TARGET_PROPERTIES(server PROPERTIES COMPILE_FLAGS "-Wall -DSERVER $ENV{CXXFLAGS}")

