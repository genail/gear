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
    race/TyreStripes.cpp
)

# Game client sources
SET(CLIENT_SRCS
    ${COMMON_SRCS}
    Application.cpp
    Game.cpp
    controllers/LoadingController.cpp
    controllers/MainMenuController.cpp
    graphics/Bound.cpp
    graphics/Car.cpp
    graphics/DecorationSprite.cpp
    graphics/DebugLayer.cpp
    graphics/GameWindow.cpp
    graphics/GroundBlock.cpp
    graphics/RaceUI.cpp
    graphics/Smoke.cpp
    graphics/SpeedMeter.cpp
    graphics/Stage.cpp
    graphics/TireTrack.cpp
    gui/MainMenuScene.cpp
    gui/LoadingScene.cpp
    gui/SceneContainer.cpp
    math/Easing.cpp
    math/Float.cpp
    race/RaceScene.cpp
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
FIND_PACKAGE(JPEG REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(Freetype REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(X11 REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

INCLUDE_DIRECTORIES(
    ${ClanLib_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${Freetype_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${Threads_INCLUDE_DIRS}
    .
)

SET(CLIENT_LIBS ${LIBS}
    ${ClanLib_App_LIBRARY}
    ${ClanLib_Core_LIBRARY}
    ${ClanLib_Display_LIBRARY}
    ${ClanLib_Network_LIBRARY}
    ${ClanLib_GUI_LIBRARY}
    ${JPEG_LIBRARY}
    ${PNG_LIBRARY}
    ${Freetype_LIBRARY}
    ${ZLIB_LIBRARY}
    ${X11_LIBRARY}
    ${Threads_LIBRARY}
)

SET(SERVER_LIBS ${LIBS}
    ${ClanLib_App_LIBRARY}
    ${ClanLib_Core_LIBRARY}
    ${ClanLib_Network_LIBRARY}
    ${Threads_LIBRARY}
)

# Game client configuration

IF (USE_GL2)
    SET(CLIENT_FLAGS "-DGL2")
    SET(CLIENT_LIBS ${CLIENT_LIBS} ${ClanLib_GL_LIBRARY})
ELSE(USE_GL2)
    SET(CLIENT_FLAGS "-DGL1")
    SET(CLIENT_LIBS ${CLIENT_LIBS} ${ClanLib_GL1_LIBRARY})
ENDIF(USE_GL2)

IF (RACE_SCENE_ONLY)
    SET(CLIENT_FLAGS "${CLIENT_FLAGS} -DRACE_SCENE_ONLY")
ENDIF(RACE_SCENE_ONLY)

ADD_EXECUTABLE(game ${CLIENT_SRCS})
TARGET_LINK_LIBRARIES(game ${CLIENT_LIBS})

SET_TARGET_PROPERTIES(
    game PROPERTIES
    LINK_FLAGS
    "-lfontconfig -lXxf86vm"
)

SET_TARGET_PROPERTIES(
    game PROPERTIES
    COMPILE_FLAGS
    "-Wall -DCLIENT ${CLIENT_FLAGS} $ENV{CXXFLAGS}"
)


# Server configuration

ADD_EXECUTABLE(server ${SERVER_SRCS})
TARGET_LINK_LIBRARIES(server ${SERVER_LIBS})

SET_TARGET_PROPERTIES(
    server PROPERTIES
    LINK_FLAGS
    "-lfontconfig"
)
SET_TARGET_PROPERTIES(
    server PROPERTIES
    COMPILE_FLAGS
    "-Wall -DSERVER $ENV{CXXFLAGS}"
)

