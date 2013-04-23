#
# CMakeLists.txt
# author: Jan Dušek <jan.dusek90@gmail.com>

include_directories(${PROJECT_SOURCE_DIR}/src/lib)

set(ZPO13_WLFCONV_HEADERS
	
)

set(ZPO13_WLFCONV_SOURCES
	main.cpp
)

add_executable(wlfconv ${ZPO13_WLFCONV_HEADERS} ${ZPO13_WLFCONV_SOURCES})
target_link_libraries(wlfconv zpo13 ${OpenCV_LIBS})