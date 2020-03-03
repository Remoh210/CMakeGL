cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)

project (CMakeGL)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

link_directories(${CMAKE_SOURCE_DIR}/lib)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

# find the required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
# find_package(SOIL REQUIRED)
# message(STATUS "Found SOIL in ${SOIL_INCLUDE_DIR}")
# find_package(GLEW REQUIRED)
# message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIR}")

if(WIN32)
  set(LIBS glfw3 opengl32 assimp)
elseif(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)
  # note that the order is important for setting the libs
  # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
  set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread ${ASSIMP_LIBRARY})
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY})
  set(LIBS ${LIBS} ${APPLE_LIBS})
else()
  set(LIBS )
endif(WIN32)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

# first create relevant static libraries requried for other projects
add_library(STB_IMAGE "src/ThirdParty/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)


set (imgui_headers ${CMAKE_SOURCE_DIR}/includes/imgui/imconfig.h ${CMAKE_SOURCE_DIR}/includes/imgui/imgui.h ${CMAKE_SOURCE_DIR}/includes/imgui/imgui_impl_glfw.h 
${CMAKE_SOURCE_DIR}/includes/imgui/imgui_impl_opengl3.h ${CMAKE_SOURCE_DIR}/includes/imgui/imgui_internal.h ${CMAKE_SOURCE_DIR}/includes/imgui/imstb_rectpack.h ${CMAKE_SOURCE_DIR}/includes/imgui/imstb_textedit.h ${CMAKE_SOURCE_DIR}/includes/imgui/imstb_truetype.h)
set (imgui_sources ${CMAKE_SOURCE_DIR}/includes/imgui/imgui.cpp ${CMAKE_SOURCE_DIR}/includes/imgui/imgui_demo.cpp ${CMAKE_SOURCE_DIR}/includes/imgui/imgui_draw.cpp ${CMAKE_SOURCE_DIR}/includes/imgui/imgui_impl_glfw.cpp ${CMAKE_SOURCE_DIR}/includes/imgui/imgui_impl_opengl3.cpp ${CMAKE_SOURCE_DIR}/includes/imgui/imgui_widgets.cpp)
add_library(ImGui STATIC ${imgui_sources} ${imgui_headers})

add_library(GLAD "src/thirdParty/glad.c")
set(LIBS ${LIBS} GLAD)

#add_library(IMGUI "src/thirdParty/glad.c" scr2.cpp ... scrx.cpp)

macro(makeLink src dest target)
  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}  DEPENDS  ${dest} COMMENT "mklink ${src} -> ${dest}")
endmacro()


# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/*.c
  ${CMAKE_SOURCE_DIR}/src/*.cpp
  ${CMAKE_SOURCE_DIR}/src/*.fs
  ${CMAKE_SOURCE_DIR}/src/*.gs
  ${CMAKE_SOURCE_DIR}/src/*.vs)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp)

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${LIBS})
if(WIN32)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}")
elseif(UNIX AND NOT APPLE)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}")
elseif(APPLE)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}")
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}")
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME}")
endif(WIN32)

file(GLOB_RECURSE SHADERS
${CMAKE_SOURCE_DIR}/src/*.fs
${CMAKE_SOURCE_DIR}/src/*.vs
${CMAKE_SOURCE_DIR}/src/*.gs
)

file(GLOB_RECURSE DLLS
${CMAKE_SOURCE_DIR}/dlls/*.dll
)

foreach(SHADER ${SHADERS})
if(WIN32)
    # configure_file(${SHADER} "test")
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
elseif(UNIX AND NOT APPLE)
    file(COPY ${SHADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/${PROJECT_NAME})
elseif(APPLE)
    # create symbolic link for *.vs *.fs *.gs
    get_filename_component(SHADERNAME ${SHADER} PROJECT_NAME)
    makeLink(${SHADER} ${CMAKE_CURRENT_BINARY_DIR}/bin/${CHAPTER}/${SHADERNAME} ${PROJECT_NAME})
endif(WIN32)
endforeach(SHADER)

# Copy DLLs
if(WIN32)
foreach(DLL ${DLLS})
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${DLL} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach(DLL)
endif(WIN32)

# Create virtual folders to make it look nicer in VS
if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources(src)
endif()

if(MSVC)
configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user @ONLY)
endif(MSVC)



include_directories(${CMAKE_SOURCE_DIR}/includes)
