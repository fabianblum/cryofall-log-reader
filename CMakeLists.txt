# CMakeList.txt: CMake-Projekt für "LogReaderCmake". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
project ("LogReader")
cmake_minimum_required (VERSION 3.8)


#option(CURL_STATICLIB "Set to ON to build libcurl with static linking."  ON)
add_executable(${PROJECT_NAME} LogReaderCmake.h LogReaderCmake.cpp "update.h"  "http.h"   "configuration.h"  "file.h"  "LogFileParser.h"  "server.h"  "common.h"  "Lock.h" "api.h")

LINK_DIRECTORIES(curl)
target_link_libraries(${PROJECT_NAME} curl stdc++fs)

# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.
set_property (TARGET ${PROJECT_NAME}
  PROPERTY
    # Enable C++17 standard compliance
    CXX_STANDARD 17
)


