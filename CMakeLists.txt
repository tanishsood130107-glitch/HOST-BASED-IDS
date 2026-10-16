
cmake_minimum_required(VERSION 3.16)
project(HIDS_Dashboard LANGUAGES CXX)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")
 
if(NOT EXISTS "${IMGUI_DIR}/imgui.cpp")
    message(FATAL_ERROR "Dear ImGui not found in third_party/imgui")
endif()
 
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Threads REQUIRED)
 
# Backend files live next to this CMakeLists.txt.
# NOT included on purpose (they each define their own main()):
#   main.cpp, OngoingProcessMonitorTest.cpp
# HIDS.cpp is not needed either: HIDSBackend replaces it.
add_executable(HIDS_Dashboard
    # UI
    HIDSDashboard.cpp
 
    # Backend
    HIDSBackend.cpp
    Alert.cpp
    Event.cpp               # your file may be named EVENT.cpp; Windows doesn't mind
    EventCollector.cpp
    EventFilter.cpp
    FileMonitor.cpp
    LoginMonitor.cpp
    OngoingProcessMonitor.cpp
    ProcessMonitor.cpp
    Rule.cpp
    RuleEngine.cpp
 
    # System Analysis modules
    SystemResourceMonitor.cpp
    ResourceHistory.cpp
    SystemAnalysis.cpp
    AnomalyDetector.cpp
 
    # Dear ImGui
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
 
# Keeps <windows.h> from defining min()/max() macros
target_compile_definitions(HIDS_Dashboard PRIVATE NOMINMAX)
 
target_link_libraries(HIDS_Dashboard PRIVATE
    glfw
    OpenGL::GL
    Threads::Threads
)
 
if(WIN32)
    # EnumProcesses, GetProcessMemoryInfo, GetModuleFileNameExA
    target_link_libraries(HIDS_Dashboard PRIVATE psapi)
endif()
 