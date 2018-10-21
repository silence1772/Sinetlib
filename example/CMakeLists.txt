# 把该目录下的所有源文件（.cpp）存入EXAMPLE_SRC_LIST
aux_source_directory(. EXAMPLE_SRC_LIST)

# 遍历所有源文件
foreach(EXAMPLE_SRC ${EXAMPLE_SRC_LIST})
	# 把xxx.cpp替换成xxx并存入EXAMPLE_EXE_NAME
	string(REGEX REPLACE "^\\./|\\.c[a-zA-Z0-9_]*$" "" EXAMPLE_EXE_NAME ${EXAMPLE_SRC})
	# 生成可执行文件
	add_executable(${EXAMPLE_EXE_NAME} ${EXAMPLE_SRC})
	# 链接到我们的动态库Sinetlib_shared
	target_link_libraries(${EXAMPLE_EXE_NAME} ${CMAKE_PROJECT_NAME}_shared)
endforeach(EXAMPLE_SRC ${EXAMPLE_SRC_LIST})