# for each "src/x.cpp", generate target "x"
# file(GLOB_RECURSE all_srcs CONFIGURE_DEPENDS *.cpp)
# foreach(v ${all_srcs})
#     string(REGEX MATCH "src/.*" relative_path ${v})
#     # message(${relative_path})
#     string(REGEX REPLACE "src/" "" target_name ${relative_path})
#     string(REGEX REPLACE "bak/" "" target_name ${target_name})
#     string(REGEX REPLACE ".cpp" "" target_name ${target_name})

#     add_executable(${target_name} ${v})
#     target_link_libraries(${target_name} pthread)
# endforeach()

# add_executable(main mldp_A4.cpp dpml.cpp global.cpp util.cpp)
# target_link_libraries(main pthread)

add_executable(main main.cpp dpml.cpp conf.cpp data.cpp path.cpp util.cpp)
target_link_libraries(main pthread)

add_custom_command(
    TARGET main
    COMMAND rm ${PROJECT_BINARY_DIR}/config && ln -s ${PROJECT_SOURCE_DIR}/config ${PROJECT_BINARY_DIR}/config
    COMMAND rm ${PROJECT_BINARY_DIR}/data && ln -s ${PROJECT_SOURCE_DIR}/data ${PROJECT_BINARY_DIR}/data
    COMMENT "Creating symbolic link to config directory in build directory"
)

