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
#include <cassert>

void printUsage() {
	std::cout << "psnr ORGINAL COMPRESSED\n";
}

double meanSquaredError(const cv::Mat& img, const cv::Mat& approx) {
	assert(img.type() == CV_8U && approx.type() == CV_8U && img.size() == approx.size());

	double sum = 0.0;
	for (int y = 0; y < img.rows; y++) {
		double rowSum = 0.0;
		for (int x = 0; x < img.cols; x++) {
			auto diff = img.at<uchar>(y, x) - approx.at<uchar>(y, x);
			rowSum += diff * diff;
		}
		sum += rowSum;
	}

	return sum / (img.rows * img.cols);
}

double computePsnr(const cv::Mat& img, const cv::Mat& approx) {
	return 10 * log10((255 * 255) / meanSquaredError(img, approx));
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printUsage();
		return 1;
	}

	cv::Mat orginal = cv::imread(argv[1]);
	if (!orginal.data)
		throw std::runtime_error("Unable to read input\"" + std::string(argv[1]) + "\"");

	cv::Mat compressed;
	try {
		compressed = WlfImage::read(argv[2]);
	} catch (std::exception&) {
		compressed = cv::imread(argv[2]);
		if (!compressed.data)
			throw std::runtime_error("Unable to read input\"" + std::string(argv[2]) + "\"");
	}

	std::cout << computePsnr(orginal.reshape(1), compressed.reshape(1)) << std::endl;

	return 0;
}