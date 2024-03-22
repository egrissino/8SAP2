cmake_minimum_required(VERSION 3.13.0)

#project 8SAP
project (8SAP LANGUAGES CXX)
# set(TARGET ${PROJECT_NAME})

set (CMAKE_CXX_STANDARD 11)

set(TARGET_NAME ${PROJECT_NAME}.exe)
add_executable(${TARGET_NAME} ${SOURCES})

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

set(CONFIG_TEST_BENCH 0)

add_subdirectory(lib/DigitalCircuitSim)
add_subdirectory(app)

if (CONFIG_TEST_BENCH)
#     add_subdirectory(test)
    add_compile_definitions(TEST_BENCH)

    target_sources(${TARGET_NAME} PUBLIC
        # ${CMAKE_CURRENT_LIST_DIR}/test/testbench.cpp
        ${CMAKE_CURRENT_LIST_DIR}/test/sim_test.cpp
    )
endif()




