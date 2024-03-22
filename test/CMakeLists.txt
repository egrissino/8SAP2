# cmake_minimum_required(VERSION 3.13.0)

#project 8SAP
# project (8SAP)

# set (CMAKE_CXX_STANDARD 11)

add_library(test "")

target_include_directories(test PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_sources(test PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/testbench.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sim_test.cpp
)


# add_compile_options(8SAP_test PUBLIC
#     ${CMAKE_CURRENT_LIST_DIR}
# )


# add_executable(${PROJECT_NAME}.exe ${SOURCE_FILES})

# ExternalProject_Add(8SAP
#     PREFIX ${CMAKE_CURRENT_LIST_DIR}/8SAP
# )
# add_dependencies(8SAP_test.exe 8SAP)
