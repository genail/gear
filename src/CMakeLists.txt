MACRO(JOIN_LIST p_list)
    SET(JOIN_LIST_RESULT "")
    
    FOREACH(element ${ARGN})
        SET(JOIN_LIST_RESULT "${JOIN_LIST_RESULT} ${element}")
    ENDFOREACH()
    
ENDMACRO(JOIN_LIST)

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
    network/packets/RaceStart.cpp
    network/packets/VoteEnd.cpp
    network/packets/VoteStart.cpp
    network/packets/VoteTick.cpp
    logic/race/level/Bound.cpp
    logic/race/level/Checkpoint.cpp
    logic/race/level/Level.cpp
    logic/race/level/Sandpit.cpp
    logic/race/level/Track.cpp
    logic/race/level/TrackPoint.cpp
    logic/race/level/TrackSegment.cpp
    logic/race/level/TrackTriangulator.cpp
    logic/race/Block.cpp
    logic/race/Car.cpp
    logic/race/MessageBoard.cpp
    logic/race/RaceLogic.cpp
    logic/race/ScoreTable.cpp
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
	controllers/OptionsController.cpp
    debug/RaceSceneKeyBindings.cpp
    gfx/DebugLayer.cpp
    gfx/DirectScene.cpp
    gfx/GameWindow.cpp
    gfx/GuiScene.cpp
    gfx/Stage.cpp
    gfx/Viewport.cpp
    gfx/race/RaceGraphics.cpp
    gfx/race/level/Bound.cpp
    gfx/race/level/Car.cpp
    gfx/race/level/DecorationSprite.cpp
    gfx/race/level/GroundBlock.cpp
    gfx/race/level/Level.cpp
    gfx/race/level/Sandpit.cpp
    gfx/race/level/Smoke.cpp
    gfx/race/level/TireTrack.cpp
    gfx/race/ui/Label.cpp
    gfx/race/ui/GameMenu.cpp
    gfx/race/ui/PlayerList.cpp
    gfx/race/ui/RaceUI.cpp
    gfx/race/ui/SpeedMeter.cpp
    gfx/scenes/MainMenuScene.cpp
    gfx/scenes/RaceScene.cpp
	gfx/scenes/OptionsScene.cpp
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
    network/server/VoteSystem.cpp
)

SET(TEST_SRCS
    # tested classes
    network/server/VoteSystem.cpp
    
    # test code
    tests/suite.cpp
    tests/network/server/VoteSystemTest.cpp
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
	.
)

SET(GEAR_LIBS ${LIBS}
    ${ClanLib_App_LIBRARY}
    ${ClanLib_Core_LIBRARY}
    ${ClanLib_Display_LIBRARY}
    ${ClanLib_Network_LIBRARY}
    ${ClanLib_GUI_LIBRARY}
	${ClanLib_GL_LIBRARY}
	${ClanLib_GL1_LIBRARY}
    ${JPEG_LIBRARY}
    ${PNG_LIBRARY}
    ${FREETYPE_LIBRARY}
    ${ZLIB_LIBRARY}
    ${X11_LIBRARY}
)

SET(SERVER_LIBS ${LIBS}
    ${ClanLib_App_LIBRARY}
    ${ClanLib_Core_LIBRARY}
    ${ClanLib_Network_LIBRARY}
	${ZLIB_LIBRARY}
)

# Platform specifics
SET(COMMON_COMPILE_FLAGS " ")

SET(GEAR_COMPILE_FLAGS " ")
SET(GEAR_LINK_FLAGS " ")

SET(SERVER_COMPILE_FLAGS " ")
SET(SERVER_LINK_FLAGS " ")

IF (CMAKE_COMPILER_IS_GNUCXX)

# -Wuninitialized
# -Wunreachable-code
# -Winline
# -Weffc++
SET(COMMON_COMPILE_FLAGS
    "-isystem/usr/local/include/ClanLib-2.1"
    "-Woverloaded-virtual"
    "-Wsign-promo"
    "-Wextra"
    "-Wunused-function"
    "-Wunused-label"
    "-Wunused-variable"
    "-Wunused-value"
    "-Wchar-subscripts"
    "-Wcomment"
    "-Wformat"
    "-Wformat-security"
    "-Winit-self"
    "-Wmain"
    "-Wmissing-include-dirs"
    "-Wparentheses"
    "-Wsequence-point"
    "-Wreturn-type"
    "-Wswitch"
    "-Wswitch-default"
    "-Wswitch-enum"
    "-Wtrigraphs"
    "-Wunknown-pragmas"
    "-Warray-bounds"
    "-Wshadow"
    "-Wlarger-than=1024"
    "-Wenum-compare"
    "-Wvolatile-register-var"
)

SET (GEAR_LINK_FLAGS "-lfontconfig -lXxf86vm ${CMAKE_THREAD_LIBS_INIT}")
SET (SERVER_LINK_FLAGS "-lfontconfig ${CMAKE_THREAD_LIBS_INIT}")

ELSEIF (MSVC) # Microsoft Visual Studio

	FIND_LIBRARY (DINPUT8_LIBRARY NAMES dinput8.lib)
	FIND_LIBRARY (DXGUID_LIBRARY NAMES dxguid.lib)
	FIND_LIBRARY (GETTEXT_LIBRARY NAMES libgettextlib.lib)
	FIND_LIBRARY (INTL_LIBRARY NAMES libintl.lib)
	
	SET (GEAR_LIBS ${GEAR_LIBS}
		${DINPUT8_LIBRARY}
		${DXGUID_LIBRARY}
		${GETTEXT_LIBRARY}
		${INTL_LIBRARY}
	)
	
	SET (SERVER_LIBS ${SERVER_LIBS}
		${ClanLib_Display_LIBRARY}
		${JPEG_LIBRARY}
		${PNG_LIBRARY}
		${FREETYPE_LIBRARY}
		${DINPUT8_LIBRARY}
		${DXGUID_LIBRARY}
		${GETTEXT_LIBRARY}
		${INTL_LIBRARY}
	)

	SET (GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} /MT")
	SET (SERVER_COMPILE_FLAGS "${SERVER_COMPILE_FLAGS} /MT")
	
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

JOIN_LIST("" ${COMMON_COMPILE_FLAGS})
SET(COMMON_COMPILE_FLAGS "${JOIN_LIST_RESULT}")

IF (UNIX)
    SET (COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -DUNIX")
ELSEIF (WIN32)
    SET (COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -DWINDOWS")
ELSEIF (APPLE)
    SET (COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -DAPPLE")
ELSE (UNIX)
    MESSAGE (FATAL_ERROR "Unsuppored platform")
ENDIF (UNIX)


# Game client configuration
IF (USE_GL2)
    SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DGL2")
ELSE(USE_GL2)
    SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DGL1")
ENDIF(USE_GL2)

IF (DRAW_CAR_VECTORS)
    SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DDRAW_CAR_VECTORS")
ENDIF (DRAW_CAR_VECTORS)

IF (DRAW_CHECKPOINTS)
    SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DDRAW_CHECKPOINTS")
ENDIF (DRAW_CHECKPOINTS)

IF (RACE_SCENE_ONLY)
    SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DRACE_SCENE_ONLY")
ENDIF(RACE_SCENE_ONLY)

ADD_EXECUTABLE(game WIN32 ${CLIENT_SRCS})
TARGET_LINK_LIBRARIES(game ${GEAR_LIBS})

SET_TARGET_PROPERTIES(
    game PROPERTIES
    LINK_FLAGS
    ${GEAR_LINK_FLAGS}
)

SET_TARGET_PROPERTIES(
    game PROPERTIES
    COMPILE_FLAGS
    "-DCLIENT ${GEAR_COMPILE_FLAGS} $ENV{CXXFLAGS} ${COMMON_COMPILE_FLAGS}"
)


# Server configuration

ADD_EXECUTABLE(server WIN32 ${SERVER_SRCS})
TARGET_LINK_LIBRARIES(server ${SERVER_LIBS})

SET_TARGET_PROPERTIES(
    server PROPERTIES
    LINK_FLAGS
    ${SERVER_LINK_FLAGS}
)
SET_TARGET_PROPERTIES(
    server PROPERTIES
    COMPILE_FLAGS
    "-DSERVER $ENV{CXXFLAGS} ${COMMON_COMPILE_FLAGS} ${SERVER_COMPILE_FLAGS}"
)

# Test configuration

ADD_EXECUTABLE(test_suite ${TEST_SRCS})
TARGET_LINK_LIBRARIES(test_suite ${GEAR_LIBS} ${SERVER_LIBS})

SET_TARGET_PROPERTIES(
    test_suite PROPERTIES
    LINK_FLAGS
    "-lfontconfig -lXxf86vm -lboost_unit_test_framework-mt"
)

SET_TARGET_PROPERTIES(
    test_suite PROPERTIES
    COMPILE_FLAGS
    "-Wall -DCLIENT $ENV{CXXFLAGS}  ${COMMON_COMPILE_FLAGS}"
)

ENABLE_TESTING()
ADD_TEST(suite test_suite)
