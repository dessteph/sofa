cmake_minimum_required(VERSION 3.12)

project(MultiThreading_test)

set ( HEADER_FILES
)
set(SOURCE_FILES
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} Sofa.Testing Sofa.Simulation.Core)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
