cmake_minimum_required(VERSION 3.16)
project(HIDS_Dashboard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")

if(NOT EXISTS "${IMGUI_DIR}/imgui.cpp")
    message(FATAL_ERROR "Dear ImGui not found in third_party/imgui")
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Threads REQUIRED)

add_executable(HIDS_Dashboard
    HIDSDashboard.cpp

    HIDSBackend.cpp
    FileScanner.cpp
    PasswordLock.cpp
    Alert.cpp
    Event.cpp
    EventCollector.cpp
    EventFilter.cpp
    FileMonitor.cpp
    LoginMonitor.cpp
    OngoingProcessMonitor.cpp
    ProcessMonitor.cpp
    Rule.cpp
    RuleEngine.cpp

    SystemResourceMonitor.cpp
    ResourceHistory.cpp
    SystemAnalysis.cpp
    AnomalyDetector.cpp

    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

target_include_directories(HIDS_Dashboard PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

target_compile_definitions(HIDS_Dashboard PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

target_link_libraries(HIDS_Dashboard PRIVATE
    glfw
    OpenGL::GL
    Threads::Threads
)

if(WIN32)
    target_link_libraries(HIDS_Dashboard PRIVATE psapi)
endif()