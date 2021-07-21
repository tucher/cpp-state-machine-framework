cmake_minimum_required(VERSION 3.0.0)
project(StateMachineFrameworkTests VERSION 0.1.0)


add_executable(${CMAKE_PROJECT_NAME} "")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
file(GLOB THIS_SOURCES "./*.cpp")
file(GLOB THIS_HEADERS "./*.h")

target_sources(${CMAKE_PROJECT_NAME} PUBLIC  ${THIS_SOURCES} ${THIS_HEADERS} )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD 17)

