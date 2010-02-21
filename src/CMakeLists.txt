MACRO(JOIN_LIST p_list)
	SET(JOIN_LIST_RESULT "")
	
	FOREACH(element ${ARGN})
		SET(JOIN_LIST_RESULT "${JOIN_LIST_RESULT} ${element}")
	ENDFOREACH()
	
ENDMACRO(JOIN_LIST)

# Common source files
SET(COMMON_SRCS
	common/Collections.cpp
	common/Game.cpp
	common/Player.cpp
	common/Properties.cpp
    common/RemotePlayer.cpp
    math/Integer.cpp
    math/Time.cpp
	network/RemoteCar.cpp
	network/client/Client.cpp
	network/packets/CarState.cpp
	network/packets/ClientInfo.cpp
	network/packets/GameState.cpp
	network/packets/Goodbye.cpp
	network/packets/PlayerJoined.cpp
	network/packets/PlayerLeft.cpp
	network/packets/RaceStart.cpp
	network/packets/VoteEnd.cpp
	network/packets/VoteStart.cpp
	network/packets/VoteTick.cpp
	logic/race/Block.cpp
	logic/race/Car.cpp
	logic/race/MessageBoard.cpp
	logic/race/Progress.cpp
	logic/race/RaceLogic.cpp
	logic/race/ScoreTable.cpp	
	logic/race/level/Bound.cpp
	logic/race/level/Checkpoint.cpp
	logic/race/level/Level.cpp
	logic/race/level/Object.cpp
	logic/race/level/Sandpit.cpp
	logic/race/level/Track.cpp
	logic/race/level/TrackPoint.cpp
	logic/race/level/TrackSegment.cpp
	logic/race/level/TrackTriangulator.cpp
	logic/race/resistance/Circle.cpp
	logic/race/resistance/Geometry.cpp
	logic/race/resistance/Primitive.cpp
	logic/race/resistance/Rectangle.cpp
	logic/race/resistance/ResistanceMap.cpp
    math/Float.cpp
    math/Easing.cpp
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
	gfx/Overlay.cpp
	gfx/Stage.cpp
	gfx/Viewport.cpp
	gfx/race/RaceGraphics.cpp
	gfx/race/level/Bound.cpp
	gfx/race/level/Car.cpp
	gfx/race/level/DecorationSprite.cpp
	gfx/race/level/Level.cpp
	gfx/race/level/Sandpit.cpp
	gfx/race/level/Smoke.cpp
	gfx/race/level/TyreStripes.cpp
	gfx/race/ui/Label.cpp
	gfx/race/ui/GameMenu.cpp
	gfx/race/ui/PlayerList.cpp
	gfx/race/ui/RaceUI.cpp
	gfx/race/ui/ScoreTable.cpp
	gfx/race/ui/SpeedMeter.cpp
	gfx/scenes/MainMenuScene.cpp
	gfx/scenes/RaceScene.cpp
	gfx/scenes/OptionsScene.cpp
	logic/race/OfflineRaceLogic.cpp
	logic/race/OnlineRaceLogic.cpp
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
	gfx/DebugLayer.cpp
	gfx/Stage.cpp
	gfx/race/ui/Label.cpp
	logic/race/Car.cpp
	logic/race/level/Object.cpp
	math/Easing.cpp
	math/Float.cpp
	math/Integer.cpp
	network/server/VoteSystem.cpp
	
	# test code
	tests/suite.cpp
	tests/common/WorkaroundsTest.cpp
	tests/logic/race/CarTest.cpp
	tests/logic/race/level/ObjectTest.cpp
	tests/math/FloatTest.cpp
	tests/math/IntegerTest.cpp
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
	${ClanLib_Display_LIBRARY}
	${ClanLib_Network_LIBRARY}
	${ZLIB_LIBRARY}
	${X11_LIBRARY}
)

SET(TEST_LIBS ${LIBS}
	${ClanLib_Core_LIBRARY}
	${ClanLib_Display_LIBRARY}
	${ClanLib_Network_LIBRARY}
	${ZLIB_LIBRARY}
	${X11_LIBRARY}
)

######################
# Platform specifics #
######################

SET(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} $ENV{CXXFLAGS}")
SET(COMMON_LINK_FLAGS "${COMMON_LINK_FLAGS} -lfontconfig -lXxf86vm")

IF (CMAKE_COMPILER_IS_GNUCXX)

# GCC warnings configuration

# -Wuninitialized
# -Wunreachable-code
# -Weffc++
SET(COMMON_COMPILE_FLAGS
	"${COMMON_COMPILE_FLAGS}"
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
	"-Winline"
)

SET (GEAR_LINK_FLAGS "${GEAR_LINK_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
SET (SERVER_LINK_FLAGS "${SERVER_LINK_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")

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

	SET (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	
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


SET(GEAR_LINK_FLAGS "${GEAR_LINK_FLAGS} ${COMMON_LINK_FLAGS}")
SET(SERVER_LINK_FLAGS "${SERVER_LINK_FLAGS} ${COMMON_LINK_FLAGS}")
SET(TEST_LINK_FLAGS "${TEST_LINK_FLAGS} -lboost_unit_test_framework-mt ${COMMON_LINK_FLAGS}")

SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DCLIENT ${COMMON_COMPILE_FLAGS}")
SET(SERVER_COMPILE_FLAGS "${SERVER_COMPILE_FLAGS} -DSERVER ${COMMON_COMPILE_FLAGS}")
SET(TEST_COMPILE_FLAGS "${TEST_COMPILE_FLAGS} -DTEST ${COMMON_COMPILE_FLAGS}")


#######################
# Configure variables #
#######################

# Vertical synchronization
IF (VSYNC)
	SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DVSYNC")
ELSE (VSYNC)
	SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DNO_VSYNC")
ENDIF (VSYNC)

# Drawing car vectors
IF (DRAW_CAR_VECTORS)
	SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DDRAW_CAR_VECTORS")
ENDIF (DRAW_CAR_VECTORS)

# Drawing level checkpoints
IF (DRAW_CHECKPOINTS)
	SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DDRAW_CHECKPOINTS")
ENDIF (DRAW_CHECKPOINTS)

# Drawing level wireframe
IF (DRAW_WIREFRAME)
	SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DDRAW_WIREFRAME")
ENDIF (DRAW_WIREFRAME)

# Drawing smokes
IF (DRAW_NO_SMOKES)
	SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DNO_SMOKES")
ENDIF (DRAW_NO_SMOKES)

IF (RACE_SCENE_ONLY)
	SET(GEAR_COMPILE_FLAGS "${GEAR_COMPILE_FLAGS} -DRACE_SCENE_ONLY")
ENDIF(RACE_SCENE_ONLY)


#########################
# Target configurations #
#########################

# game configuration

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
	"${GEAR_COMPILE_FLAGS}"
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
	"${SERVER_COMPILE_FLAGS}"
)

# Test configuration

ADD_EXECUTABLE(test_suite ${TEST_SRCS})
TARGET_LINK_LIBRARIES(test_suite ${TEST_LIBS})

SET_TARGET_PROPERTIES(
	test_suite PROPERTIES
	LINK_FLAGS
	${TEST_LINK_FLAGS}
)

SET_TARGET_PROPERTIES(
	test_suite PROPERTIES
	COMPILE_FLAGS
	"${TEST_COMPILE_FLAGS}"
)

ENABLE_TESTING()
ADD_TEST(suite test_suite)
