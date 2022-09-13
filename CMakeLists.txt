cmake_minimum_required(VERSION 3.12)
project(ultimatetapankaikki VERSION 1.21.3)

# EMCC can retrieve SDL2 by itself
if (NOT EMSCRIPTEN)
	if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2.cmake")
		file(DOWNLOAD "https://github.com/tcbrindle/sdl2-cmake-scripts/raw/master/FindSDL2.cmake" "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2.cmake")
	endif()
	if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2_mixer.cmake")
		file(DOWNLOAD "https://github.com/tcbrindle/sdl2-cmake-scripts/raw/master/FindSDL2_mixer.cmake" "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2_mixer.cmake")
	endif()
	if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2_image.cmake")
		file(DOWNLOAD "https://github.com/tcbrindle/sdl2-cmake-scripts/raw/master/FindSDL2_image.cmake" "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2_image.cmake")
	endif()
	if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2_net.cmake")
		file(DOWNLOAD "https://github.com/veikkos/sdl2-cmake-scripts/raw/master/FindSDL2_net.cmake" "${CMAKE_BINARY_DIR}/cmake-modules/FindSDL2_net.cmake")
	endif()

	find_package(SDL2 REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_net REQUIRED)
else()
	set(TK_LOBBY_HOST "localhost:3000" CACHE STRING "Hostname of the lobby server")
	set(COMPILE_FLAGS "-O3 -g3 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 -s SDL2_IMAGE_FORMATS=[png]")
	set(LINKER_FLAGS "-s TOTAL_MEMORY=32MB -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=[$ccall] -s ASYNCIFY -s ASYNCIFY_IMPORTS=[webrtc_create_server,webrtc_update_server,webrtc_connect_to_server,webrtc_connect_to_lobby,webrtc_get_server_list] --js-library ${CMAKE_CURRENT_SOURCE_DIR}/SRC/NET/TRANSPORT/WEBRTC/TRANSPORT.JS --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/FNTS@/FNTS --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/EFPS@/EFPS --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/LEVS@/LEVS --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/WAVS@/WAVS --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/MUSIC_OGG@MUSIC_OGG/ --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/OPTIONS.CFG@/OPTIONS.CFG --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/SRC/WEB/shell.html")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILE_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COMPILE_FLAGS} ${LINKER_FLAGS} -lidbfs.js")
	set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

include(GNUInstallDirs)
if (NOT WIN32 AND NOT EMCC AND NOT (${CMAKE_BUILD_TYPE} STREQUAL "Debug"))
	set(TK_DATADIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
else()
	set(TK_DATADIR ".")
endif()
set(TK_SUFFIXED_DATADIR ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

if (CMAKE_MACOSX_BUNDLE)
	set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME Runtime)
	set(CMAKE_INSTALL_BINDIR ".")  # no bin/ directory under destination directory
	set(CMAKE_INSTALL_DATADIR ${CMAKE_INSTALL_BINDIR}/${PROJECT_NAME}.app/Contents/Resources)
	set(CMAKE_INSTALL_DOCDIR ${CMAKE_INSTALL_BINDIR}/${PROJECT_NAME}.app/Contents/Documentation)
	set(TK_SUFFIXED_DATADIR ${CMAKE_INSTALL_DATADIR})  # within bundle, no suffix needed
	set(TK_MACOSX_BUNDLE 1)
	set(MACOSX_BUNDLE_IDENTIFIER "suomipelit.ultimatetapankaikki")
	set(ICON_NAME "icon.icns")
	set(ICON_PATH "icon/${ICON_NAME}")
	set(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (WIN32)
	add_compile_options(/MT /WX /wd4244 /wd4996)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funsigned-char -Wall -Wno-pedantic -Wsign-compare")
endif()

file(GLOB SOURCES
	SRC/*.H SRC/*.CPP
	SRC/EFP/*.H SRC/EFP/*.CPP
	SRC/ERROR/*.H SRC/ERROR/*.CPP
	SRC/NET/NET.CPP SRC/NET/NET.H SRC/NET/NETDEFS.CPP SRC/NET/NETDEFS.H SRC/NET/SERVERCONNECTIONRESULT.H
	SRC/NET/TRANSPORT/*.H
	SRC/INPUT/*.H SRC/INPUT/*.CPP
        CONFIG.H)

if (EMSCRIPTEN)
	file(GLOB TRANSPORT_SOURCES
		SRC/NET/TRANSPORT/WEBRTC/*.CPP
		SRC/NET/SERVERINFO.H
		SRC/NET/SERVERINFO.CPP)
else()
	file(GLOB TRANSPORT_SOURCES SRC/NET/TRANSPORT/SDL/*.CPP)
endif()
list(APPEND SOURCES ${TRANSPORT_SOURCES})

if (WIN32)
	file(GLOB MSVC_SOURCES SRC/MSVC/*.H SRC/MSVC/*.CPP)
	list(APPEND SOURCES ${MSVC_SOURCES})
endif()

add_executable(${PROJECT_NAME} ${SOURCES})

configure_file("${PROJECT_SOURCE_DIR}/CONFIG.H.IN" "${PROJECT_BINARY_DIR}/CONFIG.H")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SRC)

if (NOT EMSCRIPTEN)
	target_include_directories(${PROJECT_NAME} PUBLIC
		${SDL2_INCLUDE_DIR}
		${SDL2_MIXER_INCLUDE_DIRS}
		${SDL2_IMAGE_INCLUDE_DIRS}
		${SDL2_NET_INCLUDE_DIRS})

	target_link_libraries(${PROJECT_NAME}
		${SDL2_LIBRARY}
		${SDL2_IMAGE_LIBRARIES}
		${SDL2_MIXER_LIBRARIES}
		${SDL2_NET_LIBRARIES})
endif()

if (WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "GAME" SUFFIX ".EXE")
endif()


install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES OPTIONS.CFG DESTINATION ${TK_SUFFIXED_DATADIR})
install(DIRECTORY EFPS FNTS LEVS MUSIC WAVS icon DESTINATION ${TK_SUFFIXED_DATADIR})
install(FILES README.md README.ORIG.TXT EDITOR.TXT EPISODES.TXT LICENSE WHAT'S.NEW DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES pkg/ultimatetapankaikki.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
if (CMAKE_MACOSX_BUNDLE)
	install(FILES ${ICON_PATH} DESTINATION ${TK_SUFFIXED_DATADIR})
	INSTALL(CODE "
	set(BU_CHMOD_BUNDLE_ITEMS ON)
	include(BundleUtilities)
	fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app\" \"\" \"\")
	" COMPONENT Runtime)
	# see e.g. https://cmake.org/pipermail/cmake/2011-March/043532.html
endif()

include(CPack)
