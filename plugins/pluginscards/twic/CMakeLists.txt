cmake_minimum_required(VERSION 2.6)
project(twiccards)

file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE INCLUDE *.hpp)

add_library(
	twiccards
	SHARED

	${SRCS}
	${INCLUDE}
  )

if (WIN32)
  set(LINK_OPT liblogicalaccess)
else()
  set(LINK_OPT logicalaccess)
endif()

SET_TARGET_PROPERTIES(twiccards PROPERTIES VERSION 1.67)
  
target_link_libraries(
	twiccards
	iso7816cards
	${LINK_OPT}
)