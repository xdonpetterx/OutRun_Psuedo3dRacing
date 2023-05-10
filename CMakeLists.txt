cmake_minimum_required(VERSION 3.25)
project(OutRun_Psuedo3dRacing)

set(CMAKE_CXX_STANDARD 20)

add_executable(MyOutRun OutRun/my_outrun.cpp)
add_executable(TutorialOutrun OutRun/tutorial_outrun.cpp)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
target_link_libraries(MyOutRun sfml-graphics sfml-audio)
target_link_libraries(TutorialOutrun sfml-graphics sfml-audio)