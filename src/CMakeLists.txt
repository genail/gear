# Game client sources
SET(CLIENT_SRCS
    Application.cpp
    graphics/DebugLayer.cpp
    graphics/Stage.cpp
    race/Block.cpp
    race/Car.cpp
    race/Level.cpp
    race/RaceScene.cpp
    race/Viewport.cpp
)

@ Server sources
SET(SERVER_SRCS
    Server.cpp
)

FIND_PACKAGE(ClanLib REQUIRED)

INCLUDE_DIRECTORIES(
    ${ClanLib_INCLUDE_DIRS}
    .
)

SET(LIBS ${LIBS}
    ${ClanLib_App_LIBRARY}
    ${ClanLib_Core_LIBRARY}
    ${ClanLib_Display_LIBRARY}
    ${ClanLib_GL_LIBRARY}
)


# Game client configuration

add_executable(game ${CLIENT_SRCS})
TARGET_LINK_LIBRARIES(game ${LIBS})
SET_TARGET_PROPERTIES(game PROPERTIES LINK_FLAGS "-lz -lfontconfig -lX11 -lXxf86vm -lpng -ljpeg -lfreetype")

# Server configuration
add_executable(server ${SERVER_SRCS})
TARGET_LINK_LIBRARIES(server ${LIBS})
