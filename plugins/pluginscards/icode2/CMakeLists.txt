cmake_minimum_required(VERSION 2.6)
project(icode2cards)

file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE INCLUDE *.hpp)

add_library(
	icode2cards
	SHARED

	${SRCS}
	${INCLUDE}
  )

if (WIN32)
  set(LINK_OPT liblogicalaccess)
else()
  set(LINK_OPT logicalaccess)
endif()

SET_TARGET_PROPERTIES(icode2cards PROPERTIES VERSION 1.67)

target_link_libraries(
	icode2cards
	iso15693cards
	${LINK_OPT}
)