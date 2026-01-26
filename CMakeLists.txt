cmake_minimum_required(VERSION 3.16)
project(Am241_CCD_Sim)

# --- Find Geant4 ---
find_package(Geant4 REQUIRED ui_all vis_all)

# --- Setup Include Directories ---
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${Geant4_INCLUDE_DIRS})

# --- Define Sources ---
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# --- Add Executable ---
add_executable(Am241_Sim main.cc ${sources} ${headers})

# --- Link Libraries ---
target_link_libraries(Am241_Sim ${Geant4_LIBRARIES})

# --- Copy Macro Files to Build Directory ---
# Ensures run.mac is available next to the executable
file(GLOB macros ${PROJECT_SOURCE_DIR}/*.mac)
file(COPY ${macros} DESTINATION ${CMAKE_BINARY_DIR})

# --- Standard Install/Post-build (Optional) ---
install(TARGETS Am241_Sim DESTINATION bin)
