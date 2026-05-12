cmake_minimum_required(VERSION 3.10)
project(webserver VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)


file(GLOB TEST_SOURCES "test/*.cpp")

#message(STATUS "TEST_SOURCES : ${TEST_SOURCES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -fPIC")
file(GLOB_RECURSE SRC_SOURCES "src/*.cpp")

message(STATUS "Found library sources: ${SRC_SOURCES}") 
add_library(webserver SHARED ${SRC_SOURCES})

target_include_directories(webserver PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(webserver PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)

# cmake3 --install .
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "" FORCE)


set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)


foreach(test_file ${TEST_SOURCES})
    # 获取文件名（去掉路径和后缀）
    # 例如：把 test/print.cpp 变成 print
    get_filename_component(test_name ${test_file} NAME_WE)
    
    # message(STATUS "test_name : ${test_name}")

    # 动态生成可执行文件
    add_executable(${test_name} ${test_file})
    
    target_link_libraries(${test_name} PUBLIC logging linuxOpt pthread asynclogging mprpc pool)
    
    target_include_directories(${test_name} PUBLIC "${CMAKE_SOURCE_DIR}/include")
    target_include_directories(${test_name} PUBLIC "usr/local/include")
    target_link_directories(${test_name} PUBLIC "${CMAKE_SOURCE_DIR}/lib")

    # set()
    install(TARGETS ${test_name} DESTINATION bin)

endforeach()


