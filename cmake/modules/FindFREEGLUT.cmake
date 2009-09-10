#########################################################################
#												#
#				FreePV's Script						#
#												#
#***********************************************************************#
#  This FreePV script tries to find freeglut library and header files   #
#  FREEGLUT_INCLUDE_DIR, headers path.						#
#  FREEGLUT_LIBRARIES, libraries needed.						#
#  FREEGLUT_FOUND, either true or false.						#
#########################################################################

IF(WIN32)
	FIND_PATH(FREEGLUT_INCLUDE_DIR GL/freeglut.h ${CMAKE_INCLUDE_PATH} $ENV{include} ${OPENGL_INCLUDE_DIR})
	FIND_LIBRARY(FREEGLUT_LIBRARY NAMES freeglut_static PATH ${CMAKE_LIBRARY_PATH} $ENV{lib}) 
ELSE(WIN32)
	FIND_PATH(FREEGLUT_INCLUDE_DIR GL/freeglut.h /usr/include /usr/local/include)
	IF(NOT FREEGLUT_INCLUDE_DIR)
		FIND_PATH(FREEGLUT_INCLUDE_DIR GL/glut.h /usr/include /usr/local/include)
	ENDIF(NOT FREEGLUT_INCLUDE_DIR)
	FIND_LIBRARY( FREEGLUT_LIBRARY NAMES freeglut glut
      		/usr/lib
      		/usr/local/lib
    	)
ENDIF(WIN32)

IF (FREEGLUT_INCLUDE_DIR AND FREEGLUT_LIBRARY)
   SET(FREEGLUT_FOUND TRUE)
   SET(FREEGLUT_LIBRARIES ${FREEGLUT_LIBRARY})
ENDIF (FREEGLUT_INCLUDE_DIR AND FREEGLUT_LIBRARY)

IF(NOT FREEGLUT_FOUND)
   IF (FREEGLUT_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find FREEGLUT")
   ENDIF (FREEGLUT_FIND_REQUIRED)
ENDIF(NOT FREEGLUT_FOUND)
