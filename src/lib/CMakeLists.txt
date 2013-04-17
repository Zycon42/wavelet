#
# CMakeLists.txt
# author: Jan Dušek <jan.dusek90@gmail.com>

set(ZPO13_LIB_HEADERS
	wavelettransform.h
	utils.h
	wavelet.h
	cdf97wavelet.h
)

set(ZPO13_LIB_SOURCES
	wavelettransform.cpp
	cdf97wavelet.cpp
)

add_library(zpo13 ${ZPO13_LIB_HEADERS} ${ZPO13_LIB_SOURCES})
target_link_libraries(zpo13)