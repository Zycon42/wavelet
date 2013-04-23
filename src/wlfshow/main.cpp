/**
 * @file main.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "wlfimage.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdexcept>

void printUsage() {
	std::cout << "wlfshow IMAGE\n";
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printUsage();
		return 1;
	}

	try {
		auto img = WlfImage::read(argv[1]);
		cv::imshow(std::string("wlfshow - ") + argv[1], img);
		cv::waitKey(0);
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}