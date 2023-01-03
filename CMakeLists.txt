cmake_minimum_required(VERSION 3.0)

set(VITA_APP_NAME "CSpot")
set(VITA_TITLEID  "CSPOT0021")
set(VITA_BG_TITLEID  "CSPOT022")
set(VITA_VERSION  "01.00")

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(cspot_vita C CXX)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -std=c++20")
set(CMAKE_CXX_STANDARD 20)

# patch CSpot and bell
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/patch/Queue.h
     DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/cspot/cspot/bell/include)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/patch/ZeroconfAuthenticator.cpp
     DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/cspot/cspot/src)

# add CSpot sources
add_compile_definitions(BELL_USE_MBEDTLS)
add_compile_definitions(VITA)
add_subdirectory("cspot/cspot" ${CMAKE_CURRENT_BINARY_DIR}/cspot)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/cspot/cspot/include
  ${CMAKE_SOURCE_DIR}/cspot/bell/include
)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/API.cpp
  src/Utils.cpp
  src/Keyboard.cpp
  src/CliFile.cpp
  src/VitaAudioSink.cpp
  src/CircularBuffer.cpp
  src/Gui.cpp
  src/PlaybackScreen.cpp
  src/LoginScreen.cpp
  src/GuiUtils.cpp
)

target_link_libraries(${PROJECT_NAME}
  SceBgAppUtil_stub
  SceNotificationUtil_stub
  pthread
  cspot
  m
  SceAudio_stub
  SceShellSvc_stub
  imgui
  freetype
  pthread
  vitaGL
  vitashark
  mathneon
  SceAppMgr_stub
  SceAppUtil_stub
  SceCommonDialog_stub
  SceCtrl_stub
  SceDisplay_stub
  SceGxm_stub
  SceKernelDmacMgr_stub
  SceLibKernel_stub
  ScePower_stub
  SceShaccCg_stub
  SceShaccCgExt
  SceSysmodule_stub
  SceTouch_stub
  taihen_stub
  ${CURL_LIBRARIES}
  ${OPENSSL_LIBRARIES}
  z
)

vita_create_self(eboot.bin
  ${PROJECT_NAME}
  UNSAFE
  ATTRIBUTE 0x0000000E
  MEMSIZE 0x0
)

set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d ATTRIBUTE=17338440 -d ATTRIBUTE2=0 -d ATTRIBUTE_MINOR=17 -s CATEGORY=gdc")

vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} eboot.bin
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE common_data/sce_sys/icon0.png sce_sys/icon0.png
  FILE common_data/fa-solid-900.ttf fa-solid-900.ttf
  FILE common_data/icon_alpha.png icon_alpha.png
  FILE common_data/ProggyVector.ttf ProggyVector.ttf
  FILE common_data/cover_art.png cover_art.png
)
