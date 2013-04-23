#
# CMakeLists.txt
# author: Jan Dušek <jan.dusek90@gmail.com>

find_package(GTest)
if (GTEST_FOUND)
	enable_testing()
	
	include_directories(${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/lib)
	
	# msvc 11 has problem with gtest header file, that can be fixed with this definition
	if (MSVC11)
		add_definitions(/D "_VARIADIC_MAX=10")
	endif (MSVC11)
	
	set(ZPO13_TESTS_SOURCES
		TestCV.cpp
		TestDwt.cpp
		TestImage.cpp
		TestEzw.cpp
	)
	
	add_executable(tests ${ZPO13_TESTS_SOURCES})
	target_link_libraries(tests zpo13 ${GTEST_BOTH_LIBRARIES} ${OpenCV_LIBS})
	
	GTEST_ADD_TESTS(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests "" ${ZPO13_TESTS_SOURCES})
else()
	message("GTest not found, tests won't be available!")
endif()
