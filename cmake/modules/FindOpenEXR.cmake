# - Try to find OpenEXR
# Once done this will define
#
#  OPENEXR_FOUND - system has OpenEXR
#  OPENEXR_INCLUDE_DIR - the OpenEXR include directory
#  OPENEXR_LIBRARIES - Link these to use OpenEXR
#  OPENEXR_DEFINITIONS - Compiler switches required for using OpenEXR
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if ( OpenEXR_INCLUDE_DIR AND OpenEXR_LIBRARIES )
   # in cache already
   SET(OpenEXR_FIND_QUIETLY TRUE)
endif ( OpenEXR_INCLUDE_DIR AND OpenEXR_LIBRARIES )

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if( NOT WIN32 )
  INCLUDE(FindPkgConfig)

  pkg_check_modules(OpenEXR OpenEXR)
  pkg_check_modules(IlmBase IlmBase)

  IF(NOT OpenEXR_INCLUDE_DIR)
    FIND_PATH(OpenEXR_INCLUDE_DIR ImathVec.h /usr/include/OpenEXR)
  ENDIF(NOT OpenEXR_INCLUDE_DIR)

  find_library ( OpenEXR_LIBRARIES NAMES IlmImf )
  find_library ( Iex_LIBRARIES NAMES Iex )
  find_library ( Half_LIBRARIES NAMES Half )
  find_library ( Imath_LIBRARIES NAMES Imath )
  find_library ( IlmThread_LIBRARIES NAMES IlmThread )

  if ( IlmThread_LIBRARIES AND Imath_LIBRARIES AND Half_LIBRARIES AND Iex_LIBRARIES AND OpenEXR_LIBRARIES )
    set ( OpenEXR_FOUND FALSE )
  else ( IlmThread_LIBRARIES AND Imath_LIBRARIES AND Half_LIBRARIES AND Iex_LIBRARIES AND OpenEXR_LIBRARIES )
    set ( IlmBase_LIBRARIES ${Iex_LIBRARIES} ${Half_LIBRARIES} ${Imath_LIBRARIES} ${IlmThread_LIBRARIES} )
    set ( OpenEXR_INCLUDE_DIR ${OpenEXR_INCLUDE_DIR} ${IlmBase_INCLUDE_DIR} )
    set ( OpenEXR_LIBRARIES ${OpenEXR_LIBRARIES} ${IlmBase_LIBRARIES} )
  endif ( IlmThread_LIBRARIES AND Imath_LIBRARIES AND Half_LIBRARIES AND Iex_LIBRARIES AND OpenEXR_LIBRARIES )

endif( NOT WIN32 )
if ( WIN32 )
  if( MSVC80 )
       set( COMPILER_PATH "C:/Program\ Files/Microsoft\ Visual\ Studio\ 8/VC" )
   endif( MSVC80 )
   if( MSVC71 )
       set( COMPILER_PATH "C:/Program\ Files/Microsoft\ Visual\ Studio\ .NET\ 2003/Vc7" )
   endif( MSVC71 )
   FIND_PATH( OpenEXR_INCLUDE_DIR ImfRgbaFile.h ImfCRgbaFile.h ImfArray.h
              PATHS $ENV{OPENEXR}/include/IlmImf ${COMPILER_PATH}/PlatformSDK/Include )
   SET( openEXRnames ilmImf.lib )
   FIND_LIBRARY( OpenEXR_LIBRARIES
                 NAMES ${openEXRnames}
                 PATHS $ENV{OPENEXR}/lib ${COMPILER_PATH}/PlatformSDK/Lib )

   SET( Iex_LIBRARIESnames Iex.lib )
   FIND_LIBRARY( Iex_LIBRARIES_LIBRARIES
                 NAMES ${Iex_LIBRARIESnames}
                 PATHS $ENV{OPENEXR}/lib ${COMPILER_PATH}/PlatformSDK/Lib )
   SET( Half_LIBRARIESnames Half.lib )
   FIND_LIBRARY( Half_LIBRARIES
                 NAMES ${Half_LIBRARIESnames}
                 PATHS $ENV{OPENEXR}/lib ${COMPILER_PATH}/PlatformSDK/Lib )
   SET( Imath_LIBRARIESnames Imath.lib )
   FIND_LIBRARY( Imath_LIBRARIES
                 NAMES ${Imath_LIBRARIESnames}
                 PATHS $ENV{OPENEXR}/lib ${COMPILER_PATH}/PlatformSDK/Lib )
   SET( openEXRnames ilmImf.lib )
   FIND_LIBRARY( OpenEXR_LIBRARIES
                 NAMES ${openEXRnames}
                 PATHS $ENV{OPENEXR}/lib ${COMPILER_PATH}/PlatformSDK/Lib )
   SET( IlmThread_LIBRARIESnames IlmThread.lib )
   FIND_LIBRARY( IlmThread_LIBRARIESnames
                 NAMES ${IlmThread_LIBRARIES}
                 PATHS $ENV{OPENEXR}/lib ${COMPILER_PATH}/PlatformSDK/Lib )                 

endif (WIN32)
#FIND_PATH(OPENEXR_INCLUDE_DIR NAMES OpenEXRConfig.h
#  PATHS
#  ${_OpenEXRIncDir}
#  PATH_SUFFIXES OpenEXR
#)

#FIND_LIBRARY(OPENEXR_LIBRARIES NAMES IlmIlf
#  PATHS
#  ${_OpenEXRLinkDir}
#)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenEXR DEFAULT_MSG OpenEXR_INCLUDE_DIR OpenEXR_LIBRARIES )

# show the OPENEXR_INCLUDE_DIR and OPENEXR_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(OpenEXR_INCLUDE_DIR OpenEXR_LIBRARIES )

