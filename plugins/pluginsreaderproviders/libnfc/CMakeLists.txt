cmake_minimum_required(VERSION 2.6)
project(libnfcreaders)

if (NOT TARGET_ARCH)
	set(TARGET_ARCH "Win32")
endif()

set(INCLUDE_FOLDER 	${CMAKE_CURRENT_SOURCE_DIR}/../../pluginsreaderproviders/iso7816
					${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/desfire
					${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/mifare
					${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/mifareultralight
					${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/mifareplus
					${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/iso7816)
INCLUDE_DIRECTORIES(${INCLUDE_FOLDER})


file(GLOB SRCS *.cpp)
file(GLOB include *.hpp)

file(GLOB include_readercardadapters ${CMAKE_CURRENT_SOURCE_DIR}/readercardadapters/*.hpp)
SOURCE_GROUP("Header Files\\readercardadapters" FILES ${include_readercardadapters})

file(GLOB source_readercardadapters ${CMAKE_CURRENT_SOURCE_DIR}/readercardadapters/*.cpp)
SOURCE_GROUP("Source Files\\readercardadapters" FILES ${source_readercardadapters})

file(GLOB include_commands ${CMAKE_CURRENT_SOURCE_DIR}/commands/*.hpp)
SOURCE_GROUP("Header Files\\commands" FILES ${include_commands})

file(GLOB source_commands ${CMAKE_CURRENT_SOURCE_DIR}/commands/*.cpp)
SOURCE_GROUP("Source Files\\commands" FILES ${source_commands})


set(INCLUDE_LIB ${include} ${include_core} ${include_readercardadapters} ${include_readers} ${include_commands})
set(SOURCE_LIB ${SRCS} ${source_readercardadapters} ${source_readers} ${source_commands})

					
set (LINK_OPT)
if (UNIX)
LINK_DIRECTORIES(${LINK_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR}/../../pluginsreaderproviders/iso7816/bin
				${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/desfire/bin
				${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/mifareplus/bin
				${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/mifare/bin
				${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/mifareultralight/bin
				${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/iso7816/bin)
set(LINK_OPT pcsclite crypto boost_date_time boost_system boost_filesystem logicalaccess)
else()
LINK_DIRECTORIES(${LINK_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR}/../../pluginsreaderproviders/iso7816/bin/${TARGET_ARCH}/${VC_FOLDER_NAME}
				${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/desfire/bin/${TARGET_ARCH}/${VC_FOLDER_NAME}
				${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/mifareplus/bin/${TARGET_ARCH}/${VC_FOLDER_NAME}
				${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/mifare/bin/${TARGET_ARCH}/${VC_FOLDER_NAME}
				${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/mifareultralight/bin/${TARGET_ARCH}/${VC_FOLDER_NAME}
				${CMAKE_CURRENT_SOURCE_DIR}/../../pluginscards/iso7816/bin/${TARGET_ARCH}/${VC_FOLDER_NAME})
		
set(LINK_OPT winscard liblogicalaccess)
endif()


add_library(
	libnfcreaders
	SHARED
	
	${SOURCE_LIB}
	${INCLUDE_LIB}
  )
  
target_link_libraries(
	libnfcreaders
	desfirecards
	mifarecards
	mifarepluscards
	mifareultralightcards
	iso7816readers
	iso7816cards
	${LINK_OPT}
)
