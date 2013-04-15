#
# CMakeLists.txt
# author: Jan Dušek <jan.dusek90@gmail.com>

set(ZPO13_LIB_HEADERS
    mathutils.h
	wavelettransform.h
)

set(ZPO13_LIB_SOURCES
    mathutils.cpp
	wavelettransform.cpp
)

add_library(zpo13 ${ZPO13_LIB_HEADERS} ${ZPO13_LIB_SOURCES})
target_link_libraries(zpo13)