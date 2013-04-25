#
# CMakeLists.txt
# author: Jan Dušek <jan.dusek90@gmail.com>

include_directories(${PROJECT_SOURCE_DIR}/src/lib)

set(ZPO13_PSNR_HEADERS
	
)

set(ZPO13_PSNR_SOURCES
	main.cpp
)

add_executable(psnr ${ZPO13_PSNR_HEADERS} ${ZPO13_PSNR_SOURCES})
target_link_libraries(psnr zpo13 ${OpenCV_LIBS})