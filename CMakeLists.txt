CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Labyrinth)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# check for freeglut
FIND_PACKAGE(FREEGLUT)
IF(FREEGLUT_FOUND)
    SET(HAVE_FREEGLUT TRUE)
    SET(STATUS_FREEGLUT "OK")
    MESSAGE("Found freeglut")
ELSE(FREEGLUT_FOUND)
    SET(STATUS_FREEGLUT "not found")
    MESSAGE("Could not find glut")
ELSE(FREEGLUT_FOUND)
ENDIF(FREEGLUT_FOUND)

# check for opengl
FIND_PACKAGE(OpenGL)
IF(OPENGL_FOUND)
    SET(HAVE_OPENGL TRUE)
    SET(STATUS_OPENGL "OK")
    MESSAGE("Found OpenGL")
ELSE(OPENGL_FOUND)
    SET(STATUS_OPENGL "not found")
    MESSAGE("Could not find OpenGL")
ENDIF(OPENGL_FOUND)

# check for openexr
FIND_PACKAGE(OpenEXR)
IF(OPENEXR_FOUND)
    SET(HAVE_OPENEXR TRUE)
    SET(STATUS_OPENEXR "OK")
    MESSAGE("Found OpenEXR")
ELSE(OPENEXR_FOUND)
    SET(STATUS_OPENEXR "not found")
    MESSAGE("Could not find OpenEXR")
ENDIF(OPENEXR_FOUND)

SET(CMAKE_CXX_FLAGS "-ggdb -Wall")

FILE(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE INCLUDES ${CMAKE_SOURCE_DIR}/src/*.h)

INCLUDE_DIRECTORIES(${FREEGLUT_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${OpenEXR_INCLUDE_DIR})
LINK_LIBRARIES(labyrinth ${FREEGLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${OpenEXR_LIBRARIES})

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
"* freeglut                     : ${STATUS_FREEGLUT}\n"
"* OpenEXR                      : ${STATUS_OPENEXR}\n"
)

MESSAGE(
"If everything is OK, proceed with\n"
"make\n"
)
