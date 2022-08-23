cmake_minimum_required(VERSION 3.5)

set(MDTOOL_VERSION 1.1-0)

if(NOT DEFINED ARCH)
    set(ARCH "native")
    execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE ARCH)
    string(REGEX REPLACE "\n$" "" ARCH "${ARCH}")
endif()

if( "${ARCH}" STREQUAL "armhf")
    message("[MDTOOL] Compiling for armhf")
    set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++)
    set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)
elseif ( "${ARCH}" STREQUAL "aarm64")
    message("[MDTOOL] Compiling for aarm64")
    set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)
    set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
else ()
    message("[MDTOOL] Compiling for native architecture")
    set(CMAKE_CXX_COMPILER /usr/bin/g++)
    set(CMAKE_C_COMPILER /usr/bin/gcc)
endif()

project(mdtool-${ARCH})

set (CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic)

file(GLOB SRC "src/*.cpp")

set(CANDLE_BUILD_STATIC ON)
set(CANDLE_OMIT_EXAMPLES ON)
add_subdirectory(candle_gitlab)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} candle)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/candle_gitlab/Candle/include ${CMAKE_SOURCE_DIR}/3rd_party/mINI/src)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "mdtool")

install(TARGETS ${PROJECT_NAME} 
    DESTINATION ${CMAKE_SOURCE_DIR}/package/mdtool_${MDTOOL_VERSION}_${ARCH}/usr/local/bin)
