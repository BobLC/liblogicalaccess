cmake_minimum_required(VERSION 2.6)
project(proxlitecards)

file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE INCLUDE *.hpp)

add_library(
	proxlitecards
	SHARED

	${SRCS}
	${INCLUDE}
  )

if (WIN32)
  set(LINK_OPT liblogicalaccess)
else()
  set(LINK_OPT logicalaccess)
endif()

SET_TARGET_PROPERTIES(proxlitecards PROPERTIES VERSION 1.67)

target_link_libraries(
	proxlitecards
	${LINK_OPT}
)