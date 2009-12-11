# Common source files
SET(COMMON_SRCS
    common/Game.cpp
    common/Player.cpp
    common/Properties.cpp
    network/client/Client.cpp
    network/packets/CarState.cpp
    network/packets/ClientInfo.cpp
    network/packets/GameState.cpp
    network/packets/Goodbye.cpp
    network/packets/PlayerJoined.cpp
    network/packets/PlayerLeaved.cpp
    logic/race/Block.cpp
    logic/race/Bound.cpp
    logic/race/Car.cpp
    logic/race/Checkpoint.cpp
    logic/race/Level.cpp
    logic/race/RaceLogic.cpp
    logic/race/Sandpit.cpp
    logic/race/ScoreTable.cpp
    logic/race/Track.cpp
    logic/race/TyreStripes.cpp
    logic/race/resistance/Circle.cpp
    logic/race/resistance/Geometry.cpp
    logic/race/resistance/Primitive.cpp
    logic/race/resistance/Rectangle.cpp
    logic/race/resistance/ResistanceMap.cpp
)

# Game client sources
SET(CLIENT_SRCS
    ${COMMON_SRCS}
    Application.cpp
    controllers/GameMenuController.cpp
    controllers/MainMenuController.cpp
    debug/RaceSceneKeyBindings.cpp
    gfx/DebugLayer.cpp
    gfx/GameWindow.cpp
    gfx/Stage.cpp
    gfx/Viewport.cpp
    gfx/race/RaceGraphics.cpp
    gfx/race/level/Bound.cpp
    gfx/race/level/Car.cpp
    gfx/race/level/DecorationSprite.cpp
    gfx/race/level/GroundBlock.cpp
    gfx/race/level/Sandpit.cpp
    gfx/race/level/Smoke.cpp
    gfx/race/level/TireTrack.cpp
    gfx/race/ui/Label.cpp
    gfx/race/ui/GameMenu.cpp
    gfx/race/ui/RaceUI.cpp
    gfx/race/ui/SpeedMeter.cpp
    gfx/scenes/MainMenuScene.cpp
    gfx/scenes/RaceScene.cpp
    logic/race/OfflineRaceLogic.cpp
    logic/race/OnlineRaceLogic.cpp
    math/Easing.cpp
    math/Float.cpp
)

# Server sources
SET(SERVER_SRCS
    ${COMMON_SRCS}
    ServerApplication.cpp
    ServerConfiguration.cpp
    network/server/Server.cpp
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
    SET(CLIENT_FLAGS "${CLIENT_FLAGS} -DGL2")
    SET(CLIENT_LIBS ${CLIENT_LIBS} ${ClanLib_GL_LIBRARY})
ELSE(USE_GL2)
    SET(CLIENT_FLAGS "${CLIENT_FLAGS} -DGL1")
    SET(CLIENT_LIBS ${CLIENT_LIBS} ${ClanLib_GL1_LIBRARY})
ENDIF(USE_GL2)

IF (DRAW_CAR_VECTORS)
    SET(CLIENT_FLAGS "${CLIENT_FLAGS} -DDRAW_CAR_VECTORS")
ENDIF (DRAW_CAR_VECTORS)

IF (DRAW_CHECKPOINTS)
    SET(CLIENT_FLAGS "${CLIENT_FLAGS} -DDRAW_CHECKPOINTS")
ENDIF (DRAW_CHECKPOINTS)

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

