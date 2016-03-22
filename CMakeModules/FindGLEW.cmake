#

# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_DIR
# GLEW_LIBRARY
# GLEW_SOURCE
#

SET(GLEW_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
)

FIND_PATH(GLEW_INCLUDE_DIR GL/glew.h
	HINTS
	${GLEW}
	$ENV{GLEW}
	PATH_SUFFIXES include/GLEW include GLEW
	PATHS ${GLEW_SEARCH_PATHS}
	DOC "The directory where GL/glew.h resides"
)

FIND_FILE(GLEW_SOURCE glew.c
	HINTS
	${GLEW}
	$ENV{GLEW}
	PATH_SUFFIXES src
	PATHS ${GLEW_SEARCH_PATHS}
	DOC "The directory where GL/glew.c resides"
)

# Lookup the 64 bit libs on x64
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(GLEW_LIBRARY
    NAMES GLEW glew glew32 glew32s
		HINTS
		${GLEW}
		$ENV{GLEW}
		PATH_SUFFIXES lib64 lib
		lib/x64
    lib/Release/x64
		x86_64-w64-mingw32/lib
		PATHS ${GLEW_SEARCH_PATHS}
		DOC "The GLEW library 64bit"
	)
# On 32bit build find the 32bit libs
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(GLEW_LIBRARY
    NAMES GLEW glew glew32 glew32s
		HINTS
    ${GLEW}
		$ENV{GLEW}
		PATH_SUFFIXES lib
		lib/x86
    ib/Release/win32
		i686-w64-mingw32/lib
		PATHS ${GLEW_SEARCH_PATHS}
		DOC "The GLEW library 32bit"
	)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)

if (GLEW_INCLUDE_DIR AND EXISTS "${GLEW_INCLUDE_DIR}/GL/glew.h")

   file(STRINGS "${GLEW_INCLUDE_DIR}/GL/glew.h" GLEW_4_2 REGEX "^#define GL_VERSION_4_2.*$")
   if (GLEW_4_2)
       SET(OPENGL_4_2_FOUND TRUE)
   else ()
       message(WARNING
       "glew-1.7.0 or newer needed for supporting OpenGL 4.2 dependent features"
       )
   endif ()

   file(STRINGS "${GLEW_INCLUDE_DIR}/GL/glew.h" GLEW_4_3 REGEX "^#define GL_VERSION_4_3.*$")
   if (GLEW_4_3)
       SET(OPENGL_4_3_FOUND TRUE)
   else ()
       message(WARNING
       "glew-1.9.0 or newer needed for supporting OpenGL 4.3 dependent features"
       )
   endif ()

endif ()

if(GLEW_SOURCE)
find_package_handle_standard_args(GLEW DEFAULT_MSG
    GLEW_INCLUDE_DIR
    GLEW_SOURCE
)
else()
find_package_handle_standard_args(GLEW DEFAULT_MSG
    GLEW_INCLUDE_DIR
    GLEW_LIBRARY
)
endif()

mark_as_advanced( GLEW_FOUND )
