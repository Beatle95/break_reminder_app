# Important! Use Ninja generator on Windows (because VS cretes Debug and Release subdirectories in EXECUTABLE_OUTPUT_PATH)
cmake_minimum_required (VERSION 3.20)
set(PROJECT_NAME break_reminder)
project(${PROJECT_NAME} VERSION 1.0.0)

# generate version file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/generate/version.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

find_package(
    Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    Network)
get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

file (
    GLOB_RECURSE SRC
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

if (WIN32)
    add_executable (
        ${PROJECT_NAME} 
        WIN32
        ${SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/MainWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/PopupWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BreakTimer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_resources/main.rc)
elseif(APPLE)
    add_executable (
        ${PROJECT_NAME} 
        MACOSX_BUNDLE
        ${SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/MainWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/PopupWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BreakTimer.h)
else ()
    add_executable (
        ${PROJECT_NAME}
        ${SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/MainWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/PopupWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BreakTimer.h)
endif()

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia)

# copy resources
file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/default.css
    ${EXECUTABLE_OUTPUT_PATH}/default.css)

# copy runtime for windows
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
            "$<TARGET_FILE:${PROJECT_NAME}>"
    COMMENT "Running windeployqt..."
)
