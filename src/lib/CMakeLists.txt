#
# CMakeLists.txt
# author: Jan Dušek <jan.dusek90@gmail.com>

set(ZPO13_LIB_HEADERS
	wavelettransform.h
	utils.h
	wavelet.h
	cdf97wavelet.h
	cdf53wavelet.h
	wlfimage.h
	bitstream.h
	ezw.h
	ezwencoder.h
	ezwdecoder.h
	arithmcodec.h
	arithmencoder.h
	arithmdecoder.h
	spihtencoder.h
)

set(ZPO13_LIB_SOURCES
	wavelettransform.cpp
	cdf97wavelet.cpp
	cdf53wavelet.cpp
	wlfimage.cpp
	ezwencoder.cpp
	ezwdecoder.cpp
	arithmcodec.cpp
	arithmencoder.cpp
	arithmdecoder.cpp
	spihtencoder.cpp
)

add_library(zpo13 ${ZPO13_LIB_HEADERS} ${ZPO13_LIB_SOURCES})
target_link_libraries(zpo13)