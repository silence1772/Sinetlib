# 把该目录下的所有源文件（.cpp）存入TEST_SRC_LIST
aux_source_directory(. TEST_SRC_LIST)

# 遍历所有源文件
foreach(TEST_SRC ${TEST_SRC_LIST})
	# 把xxx.cpp替换成xxx并存入TEST_EXE_NAME
	string(REGEX REPLACE "^\\./|\\.c[a-zA-Z0-9_]*$" "" TEST_EXE_NAME ${TEST_SRC})
	# 生成可执行文件
	add_executable(${TEST_EXE_NAME} ${TEST_SRC})
	# 链接到我们的动态库Sinetlib_shared
	target_link_libraries(${TEST_EXE_NAME} ${CMAKE_PROJECT_NAME}_shared)
endforeach(TEST_SRC ${TEST_SRC_LIST})