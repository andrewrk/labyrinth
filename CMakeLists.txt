CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Labyrinth)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

# prepare for special include files
SET(OUT_INCLUDE_PATH "${CMAKE_BINARY_DIR}/cmakeincludes")

# create version number
SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "1")
IF(EXISTS ${CMAKE_SOURCE_DIR}/.svn)
    INCLUDE(FindSubversion)
    IF(Subversion_FOUND)
        EXECUTE_PROCESS(WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND ${Subversion_SVN_EXECUTABLE} info
            COMMAND grep "^Revision:"
            OUTPUT_VARIABLE SVN_REV)
        STRING(REGEX MATCH "[0-9]+" VERSION_SUFFIX "${SVN_REV}")
    ENDIF(Subversion_FOUND)
ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/.svn)
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
IF(VERSION_SUFFIX)
    SET(VERSION "${VERSION}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/include/version.h.in
    ${OUT_INCLUDE_PATH}/version.h)
MESSAGE("Compiling version ${VERSION}")

# set folders to install to
IF(WIN32)
    SET(LABYRINTH_DATA_DIR ".")
ELSE(WIN32)
    SET(LABYRINTH_DATA_DIR "share")
ENDIF(WIN32)

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

# check for ilmbase
FIND_PACKAGE(IlmBase)
IF(ILMBASE_FOUND)
    SET(HAVE_ILMBASE TRUE)
    SET(STATUS_ILMBASE "OK")
    MESSAGE("Found ilmbase")
ELSE(ILMBASE_FOUND)
    SET(STATUS_ILMBASE "not found")
    MESSAGE("Could not find ilmbase")
ENDIF(ILMBASE_FOUND)

# check for boost
#SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.36.0 COMPONENTS program_options)
IF(Boost_FOUND)
    SET(HAVE_BOOST TRUE)
    SET(STATUS_BOOST "OK")
    MESSAGE("Found boost")
ELSE(Boost_FOUND)
    SET(STATUS_BOOST "not found")
    MESSAGE("Could not find boost")
ENDIF(Boost_FOUND)

# make sub directories
ADD_SUBDIRECTORY("resources")

SET(CMAKE_CXX_FLAGS "-ggdb -Wall")

FILE(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE INCLUDES ${CMAKE_SOURCE_DIR}/src/*.h)

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
INCLUDE_DIRECTORIES(${FREEGLUT_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${ILMBASE_INCLUDE_DIR} ${OUT_INCLUDE_PATH} ${Boost_INCLUDE_DIRS})
LINK_LIBRARIES(labyrinth ${FREEGLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${ILMBASE_LIBRARIES})

ADD_EXECUTABLE(labyrinth ${SOURCES})
TARGET_LINK_LIBRARIES(labyrinth ${Boost_LIBRARIES})


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
"* ilmbase                      : ${STATUS_ILMBASE}\n"
"* Boost                        : ${STATUS_BOOST}\n"
)

MESSAGE(
"If everything is OK, proceed with\n"
"make\n"
)
