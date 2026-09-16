cmake_minimum_required(VERSION 3.14)

project(GLGE_EXAMPLES LANGUAGES CXX)

add_subdirectory(${PROJECT_SOURCE_DIR}/../ ${PROJECT_BINARY_DIR}/GLGE)

function(ADD_EXAMPLE ARG_NAME ARG_SRC)
    message(STATUS "Creating GLGE example "${ARG_NAME}" from source "${ARG_SRC})
    add_executable(${ARG_NAME} ${ARG_SRC})

    target_link_libraries(${ARG_NAME} PUBLIC GLGE)
    set_target_properties(${ARG_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
    )
endfunction()

ADD_EXAMPLE(EXAMPLE_3D src/3DExample.cpp)