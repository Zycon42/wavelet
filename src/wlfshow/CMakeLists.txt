#
# CMakeLists.txt
# author: Jan Du�ek <jan.dusek90@gmail.com>

include_directories(${PROJECT_SOURCE_DIR}/src/lib)

set(ZPO13_WLFSHOW_HEADERS
	
)

set(ZPO13_WLFSHOW_SOURCES
	main.cpp
)

add_executable(wlfshow ${ZPO13_WLFSHOW_HEADERS} ${ZPO13_WLFSHOW_SOURCES})
target_link_libraries(wlfshow zpo13 ${OpenCV_LIBS})