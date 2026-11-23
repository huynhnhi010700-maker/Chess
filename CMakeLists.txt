cmake_minimum_required(VERSION 3.21)
project(XiangqiOOP LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SFML URL https://github.com/SFML/SFML/archive/refs/tags/2.6.1.zip DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(SFML)
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(XiangqiOOP ${GAME_SOURCES})
target_link_libraries(XiangqiOOP PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)
# On Windows, use the GUI subsystem so XiangqiOOP.exe does not open a console.
# SFML 2.6 sfml-main provides the WinMain entry point and calls main().
if(WIN32)
 target_link_libraries(XiangqiOOP PRIVATE sfml-main)
 set_target_properties(XiangqiOOP PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
# Save games are stored in the project saves folder, not the Visual Studio build folder.
target_compile_definitions(XiangqiOOP PRIVATE XIANGQI_SAVE_DIRECTORY=\"${CMAKE_CURRENT_SOURCE_DIR}/saves\")
if(MSVC)
 target_compile_options(XiangqiOOP PRIVATE /W4 /utf-8)
else()
 target_compile_options(XiangqiOOP PRIVATE -Wall -Wextra)
endif()
add_custom_command(TARGET XiangqiOOP POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:XiangqiOOP>/assets"
 COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:XiangqiOOP>/saves"
 COMMAND ${CMAKE_COMMAND} -E copy -t "$<TARGET_FILE_DIR:XiangqiOOP>" $<TARGET_RUNTIME_DLLS:XiangqiOOP>
 COMMAND_EXPAND_LISTS
)
# Copy OpenAL, a runtime dependency of SFML-audio on Windows.
if(WIN32)
 find_file(OPENAL_RUNTIME NAMES OpenAL32.dll
  PATHS "${sfml_SOURCE_DIR}/extlibs/bin/x64" "${sfml_SOURCE_DIR}/extlibs/bin/x86"
  NO_DEFAULT_PATH)
 if(OPENAL_RUNTIME)
  add_custom_command(TARGET XiangqiOOP POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENAL_RUNTIME}" "$<TARGET_FILE_DIR:XiangqiOOP>/OpenAL32.dll")
 else()
  message(WARNING "OpenAL32.dll was not found in SFML extlibs; copy it manually from SFML dependencies")
 endif()
endif()
set_target_properties(XiangqiOOP PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:XiangqiOOP>")

# The visual application is the default startup target in Visual Studio.
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT XiangqiOOP)
