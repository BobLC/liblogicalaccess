cmake_minimum_required(VERSION 2.6)
project(rfideasreaders)

file(GLOB SRCS *.cpp)
file(GLOB include *.hpp)

set(INCLUDE_LIB ${include} ${include_core})
set(SOURCE_LIB ${SRCS})

add_library(
	rfideasreaders
	SHARED
	
	${SOURCE_LIB}
	${INCLUDE_LIB}
  )

if (WIN32)
  set(LINK_OPT liblogicalaccess)
else()
  set(LINK_OPT logicalaccess)
endif()

SET_TARGET_PROPERTIES(rfideasreaders PROPERTIES VERSION 1.67)  

target_link_libraries(
	rfideasreaders
	${LINK_OPT}
)