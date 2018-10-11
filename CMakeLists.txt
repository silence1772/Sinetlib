project (Sinetlib)
cmake_minimum_required (VERSION 2.8)

# 如果没有设置这个环境变量，则默认设置为Release，可在build.sh中加上-D选项设置
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

# 设置编译器
set(CMAKE_CXX_COMPILER "g++")
# 设在c++11及开启所有警告
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")
# 当编译类型为Debug时，不优化、开启调试信息、针对gdb生成更详细的调试信息
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb")
# 当编译类型为Release时，开启高级优化
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# 输出编译类型
message(STATUS "BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
# 把编译类型转换为大写存入BUILD_TYPE
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
# 输出编译选项
message(STATUS "CXXFLAGS: " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

# 设置库的输出路径，PROJECT_BINARY_DIR在此处是执行cmake的目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
# 设置可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# 设置安装库的路径
set(INSTALL_PATH_LIB lib)
# 设置安装头文件的路径
set(INSTALL_PATH_INCLUDE include)

# src下所有源码目录
set(SRC_LIST "log" "base" "net" "http")

# 遍历所有源码目录
foreach(SUB_DIR ${SRC_LIST})
	# 将该目录设置为头文件目录，不然在编译时会找不到头文件
	include_directories(${PROJECT_SOURCE_DIR}/src/${SUB_DIR})
	# 将目录下的源文件（即.cpp）存入SUB_LIST，后面会用到
	aux_source_directory(src/${SUB_DIR} SUB_LIST)
	# 将目录下的头文件（匹配*.h）安装到CMAKE_INSTALL_PREFIX/INSTALL_PATH_INCLUDE
	# 前者我们没定义，默认是/usr/local，因此实际安装路径为/usr/local/include
	# 该动作会在make执行完后执行make install时执行，实际上就是把这些头文件复制到对应目录，这样别人在使用我们的库时就可以直接像系统头文件一样使用
	install(DIRECTORY src/${SUB_DIR} DESTINATION ${INSTALL_PATH_INCLUDE} FILES_MATCHING PATTERN "*.h")
endforeach()

# 将所有源文件打包成动态库Sinetlib_shared
add_library(${CMAKE_PROJECT_NAME}_shared SHARED ${SUB_LIST})
# 链接到需要的库pthread
target_link_libraries(${CMAKE_PROJECT_NAME}_shared pthread)
# 将生成的库文件名改为Sinetlib（系统会自动扩为libSinetlib.so）
set_target_properties(${CMAKE_PROJECT_NAME}_shared PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}")
# 安装到/usr/local/lib，同样的，在执行make install时才会执行
install(TARGETS ${CMAKE_PROJECT_NAME}_shared ARCHIVE DESTINATION ${INSTALL_PATH_LIB} LIBRARY DESTINATION ${INSTALL_PATH_LIB})

# 将所有源文件打包成静态库Sinetlib_static
add_library(${CMAKE_PROJECT_NAME}_static STATIC ${SUB_LIST})
# 链接到需要的库pthread
target_link_libraries(${CMAKE_PROJECT_NAME}_shared pthread)
# 将生成的库文件名改为Sinetlib（系统会自动扩为libSinetlib.a）
set_target_properties(${CMAKE_PROJECT_NAME}_static PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}")
# 安装到/usr/local/lib，同样的，在执行make install时才会执行
install(TARGETS ${CMAKE_PROJECT_NAME}_static ARCHIVE DESTINATION ${INSTALL_PATH_LIB})

# 添加子目录，这样test目录下的CMakeLists.txt也会被执行
add_subdirectory(test)
