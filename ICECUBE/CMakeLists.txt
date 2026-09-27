cmake_minimum_required(VERSION 3.16...3.27)
project(IceCubeSim)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# Başlık dosyalarının nerede olduğunu belirt
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(IceCubeSim IceCubeSim.cc ${sources} ${headers})
target_link_libraries(IceCubeSim ${Geant4_LIBRARIES})