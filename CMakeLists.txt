CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Labyrinth)

# check for glut
FIND_PACKAGE(GLUT)
IF(GLUT_FOUND)
    SET(HAVE_GLUT TRUE)
    SET(STATUS_GLUT "OK")
    MESSAGE("Found glut")
ELSE(GLUT_FOUND)
    SET(STATUS_GLUT "not found")
    MESSAGE("Could not find glut")
ELSE(GLUT_FOUND)
ENDIF(GLUT_FOUND)

# check for opengl
FIND_PACKAGE(OpenGL)
IF(OPENGL_FOUND)
    SET(HAVE_GLUT TRUE)
    SET(STATUS_OPENGL "OK")
    MESSAGE("Found OpenGL")
ELSE(OPENGL_FOUND)
    SET(STATUS_OPENGL "not found")
    MESSAGE("Could not find OpenGL")
ENDIF(OPENGL_FOUND)

SET(CMAKE_CXX_FLAGS "-ggdb -Wall")

FILE(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE INCLUDES ${CMAKE_SOURCE_DIR}/src/*.h)

MESSAGE("include dirs: ${GLUT_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR}")
INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
LINK_LIBRARIES(labyrinth ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})

ADD_EXECUTABLE(labyrinth ${SOURCES})

MESSAGE("\n"
"Installation Summary\n"
"--------------------\n"
"* Install Directory            : ${CMAKE_INSTALL_PREFIX}\n"
)

MESSAGE(
"Required Libraries\n"
"------------------\n"
"* OpenGL                       : ${STATUS_OPENGL}\n"
"* glut                         : ${STATUS_GLUT}\n"
)

MESSAGE(
"If everything is OK, proceed with\n"
"make\n"
)
