file(GLOB GLMaze_SRC
    "*.h"
    "*.c"
    "*.m"
)

Find_Package ( OpenGL REQUIRED )
Find_Package ( SDL REQUIRED )
Find_Package ( GLEW REQUIRED )
Find_Package ( FreeImage REQUIRED )

include_directories(${GLEW_INCLUDE_DIRS})

link_libraries (
  ${OPENGL_LIBRARIES}
  ${SDL_LIBRARY}
  ${GLEW_LIBRARIES}
  ${FREEIMAGE_LIBRARY}
)

set(RESOURCES ${PROJECT_SOURCE_DIR}/res/config.txt
              ${PROJECT_SOURCE_DIR}/res/Shader
              ${PROJECT_SOURCE_DIR}/res/Textures
              ${PROJECT_SOURCE_DIR}/res/Xcode/glmaze.icns
              ${PROJECT_SOURCE_DIR}/res/Xcode/English.lproj
              ${PROJECT_SOURCE_DIR}/res/Xcode/de.lproj
              ${PROJECT_SOURCE_DIR}/res/Xcode/dsa_pub.pem)

add_executable(GLMaze MACOSX_BUNDLE ${GLMaze_SRC} ${RESOURCES})
set_source_files_properties(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_target_properties(GLMaze PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/res/Xcode/Info.plist)
