include(FetchContent)

enable_testing()

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)

file (GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

include_directories(mdtool_test ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/candle/include
                    ${CMAKE_SOURCE_DIR}/3rd_party/mINI/src)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(mdtool_test ${CMAKE_SOURCE_DIR}/src/ui.cpp ${TEST_SOURCES}
               ${CMAKE_SOURCE_DIR}/src/ConfigManager.cpp)

target_link_libraries(mdtool_test
    PRIVATE
    GTest::GTest
    candle)

add_test(mdtool_gtests mdtool_test)