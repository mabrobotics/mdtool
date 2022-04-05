cmake_minimum_required(VERSION 3.5)
project(mdtool)

set (CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic)

file(GLOB SRC "src/*.cpp")

set(CANDLE_OMIT_EXAMPLES ON)
add_subdirectory(md80_usbcan/Candle)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} candle)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/md80_usbcan/Candle/include)

install(TARGETS ${PROJECT_NAME} 
    DESTINATION ${CMAKE_SOURCE_DIR}/package/mdtools_0.1-1_amd64/usr/local/bin)