cmake_minimum_required(VERSION 3.5)

set(MDTOOL_VERSION 1.0-0)

if(NOT DEFINED ARCH)
    set(ARCH "native")
    execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE ARCH)
    string(REGEX REPLACE "\n$" "" ARCH "${ARCH}")
endif()

if( "${ARCH}" STREQUAL "armhf")
    message("Compiling for armhf")
    set(CMAKE_CXX_COMPILER arm-linux-gnueabi-g++)
    set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
elseif ( "${ARCH}" STREQUAL "aarm64")
    message("Compiling for aarm64")
    set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
else ()
    message("Compiling for native architecture")
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_C_COMPILER gcc)
endif()

project(mdtool-${ARCH})

set (CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic)

file(GLOB SRC "src/*.cpp")

set(CANDLE_OMIT_EXAMPLES ON)
add_subdirectory(candle/Candle)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} candle)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/candle/Candle/include)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "mdtool")

install(TARGETS ${PROJECT_NAME} 
    DESTINATION ${CMAKE_SOURCE_DIR}/package/mdtool_${MDTOOL_VERSION}_${ARCH}/usr/local/bin)
