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

string(REPLACE "-framework Cocoa" "" SDL_FRAMEWORK ${SDL_LIBRARY})
get_filename_component(GLEW_PATH ${GLEW_LIBRARIES} REALPATH)
get_filename_component(FREEIMAGE_PATH ${FREEIMAGE_LIBRARY} REALPATH)

set(FRAMEWORKS ${SDL_FRAMEWORK} ${GLEW_PATH} ${FREEIMAGE_PATH})
set_source_files_properties(${FRAMEWORKS} PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)

set(RESOURCES ${PROJECT_SOURCE_DIR}/res/config.txt
              ${PROJECT_SOURCE_DIR}/res/Shader
              ${PROJECT_SOURCE_DIR}/res/Textures
              ${PROJECT_SOURCE_DIR}/res/Xcode/glmaze.icns
              ${PROJECT_SOURCE_DIR}/res/Xcode/English.lproj
              ${PROJECT_SOURCE_DIR}/res/Xcode/de.lproj
              ${PROJECT_SOURCE_DIR}/res/Xcode/dsa_pub.pem)

set_source_files_properties(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)


add_executable(GLMaze MACOSX_BUNDLE ${GLMaze_SRC} ${RESOURCES} ${FRAMEWORKS})
set_target_properties(GLMaze PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/res/Xcode/Info.plist)
set_target_properties(GLMaze PROPERTIES XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path @loader_path/../Frameworks")
