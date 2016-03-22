# Add all source and header files
file(GLOB SRC
    "*.h"
    "*.c"
)

# Locate required libraries
Find_Package ( OpenGL REQUIRED )
Find_Package ( SDL2 REQUIRED )
Find_Package ( GLEW REQUIRED )
Find_Package ( FreeImage REQUIRED )

# Include GLEW Headers
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${FREEIMAGE_INCLUDE_PATH})

# Link all libraries
link_libraries (
  ${OPENGL_LIBRARIES}
  ${SDL2_LIBRARY}
  ${GLEW_LIBRARY}
  ${FREEIMAGE_LIBRARY}
)

IF(WIN32)
  # Hide console on Windows
  SET(GUI_TYPE WIN32)
ENDIF(WIN32)

if(APPLE)
  # Create a .app bundle
  SET(GUI_TYPE MACOSX_BUNDLE)

  # Get framework/library location to copy into bundle
  #! Cmake adds -framework Cocoa behind SDL framework for Mac
  string(REPLACE "-framework Cocoa" "" SDL2_FRAMEWORK ${SDL2_LIBRARY})
  get_filename_component(GLEW_PATH ${GLEW_LIBRARY} REALPATH)
  get_filename_component(FREEIMAGE_PATH ${FREEIMAGE_LIBRARY} REALPATH)

  # Copy framework/library into bundle
  set(FRAMEWORKS ${SDL2_FRAMEWORK} ${GLEW_PATH} ${FREEIMAGE_PATH})
  set_source_files_properties(${FRAMEWORKS} PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)

  # Copy resources into bundle
  set(RESOURCES ${PROJECT_SOURCE_DIR}/res/config.txt
                ${PROJECT_SOURCE_DIR}/res/Shader
                ${PROJECT_SOURCE_DIR}/res/Textures
                ${PROJECT_SOURCE_DIR}/res/Xcode/glmaze.icns
                ${PROJECT_SOURCE_DIR}/res/Xcode/English.lproj
                ${PROJECT_SOURCE_DIR}/res/Xcode/de.lproj
                ${PROJECT_SOURCE_DIR}/res/Xcode/dsa_pub.pem)

  set_source_files_properties(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

  # Set Info.plist values
  string(TIMESTAMP YEAR "%Y")

  set(MACOSX_BUNDLE_EXECUTABLE_NAME ${PROJECT_TARGETBIN})
  set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
  set(MACOSX_BUNDLE_GUI_IDENTIFIER ${PROJECT_IDENTIFIER})
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION} rev. ${PROJECT_REVISION}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")
  set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} version ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}, Copyright © 2006-${YEAR} ultitech.")
  set(MACOSX_BUNDLE_ICON_FILE "glmaze.icns")
  set(MACOSX_BUNDLE_COPYRIGHT "Copyright © 2006-${YEAR}, ultitech.\nAll rights reserved.")
endif(APPLE)

# Create Executable
add_executable(${PROJECT_TARGETBIN} ${GUI_TYPE} ${SRC} ${RESOURCES} ${FRAMEWORKS})

if(APPLE)
  # Set search paths to look for framework inside the bundle
  set_target_properties(${PROJECT_TARGETBIN} PROPERTIES XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path @loader_path/../Frameworks")
endif(APPLE)
