project (Sinetlib)
cmake_minimum_required (VERSION 2.8)

#设置编译器
#SET(CMAKE_CXX_COMPILER "g++")
#设置c++11
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

add_compile_options(-std=c++11)
add_compile_options(-Wall)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(SRC_LIST "log")
set(INSTALL_PATH_LIB lib)
set(INSTALL_PATH_INCLUDE include)

foreach(SUB_DIR ${SRC_LIST})
	aux_source_directory(src/${SUB_DIR} SUB_LIST)
	install(DIRECTORY src/${SUB_DIR} DESTINATION ${INSTALL_PATH_INCLUDE} FILES_MATCHING PATTERN "*.h")
endforeach()

include_directories(${PROJECT_SOURCE_DIR}/src/log)

add_library(${CMAKE_PROJECT_NAME}_shared SHARED ${SUB_LIST})
#target_link_libraries(${CMAKE_PROJECT_NAME}_shared pthread)
set_target_properties(${CMAKE_PROJECT_NAME}_shared PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}")
install(TARGETS ${CMAKE_PROJECT_NAME}_shared ARCHIVE DESTINATION ${INSTALL_PATH_LIB} LIBRARY DESTINATION ${INSTALL_PATH_LIB})

add_library(${CMAKE_PROJECT_NAME}_static STATIC ${SUB_LIST})
set_target_properties(${CMAKE_PROJECT_NAME}_static PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}")
install(TARGETS ${CMAKE_PROJECT_NAME}_static ARCHIVE DESTINATION ${INSTALL_PATH_LIB})


add_subdirectory(test)