### YOU CAN'T MODIFY THE CODE BELOW
cmake_minimum_required(VERSION 3.16)
project(Bookstore)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

include_directories(
	${PROJECT_SOURCE_DIR}/src/
)
	
option(WEB_MODE "Set to switch to build with web mode" OFF)

if (DEFINED WEB_MODE)
        message(STATUS "WEB_MODE defined: " ${WEB_MODE})
else ()     
        message(STATUS "WEB_MODE un-defined: " ${WEB_MODE})
endif()

if (WEB_MODE)
	message("Web mode enabled")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -DENABLE_WEB")
	find_package(jwt-cpp CONFIG REQUIRED)
else()
	message("Web mode disabled, using `-DWEB_MODE=ON` to enable it")
endif()

file(GLOB_RECURSE main_src src/*.cpp)

add_executable(code ${main_src}) 

if (WEB_MODE)
	target_link_libraries(code PRIVATE jwt-cpp::jwt-cpp)
endif()
