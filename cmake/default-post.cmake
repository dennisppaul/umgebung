# add umgebung

target_compile_definitions(${PROJECT_NAME} PRIVATE UMGEBUNG_WINDOW_TITLE="${PROJECT_NAME}") # set window title
add_subdirectory(${UMGEBUNG_PATH} ${CMAKE_BINARY_DIR}/umgebung-lib-${PROJECT_NAME})
add_umgebung_libs()

# add run target

add_custom_target(run
        COMMAND ${PROJECT_NAME}
        DEPENDS ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
