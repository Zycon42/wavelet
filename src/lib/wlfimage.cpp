/**
 * @file wlfimage.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "wlfimage.h"

#include "wavelettransform.h"
#include "cdf97wavelet.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <fstream>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <functional>

struct Header
{
	enum class PixelFormat : uint8_t {
		RGB, Gray, YCbCr444
	};

	static const char* magic;

	uint32_t width;
	uint32_t height;
	PixelFormat pf;
	uint8_t dwtLevels;
};

const char* Header::magic = "\x89\x57\x4c\x46\x0d\x0a\x1a\x0a";

using namespace std::placeholders;
std::function<void (const cv::Mat&, cv::Mat&)> colorTransforms[] = {
	std::bind(cv::cvtColor, _1, _2, CV_RGB2BGR, 0), std::bind(cv::cvtColor, _1, _2, CV_GRAY2BGR, 0),
	std::bind(cv::cvtColor, _1, _2, CV_YCrCb2BGR, 0)
};

template <typename T>
void writeElement(std::ostream& stream, const T& elm) {
	stream.write(reinterpret_cast<const char*>(&elm), sizeof(elm));
	if (!stream)
		throw std::runtime_error("Unable to write element to stream");
}

static void writeHeader(std::ostream& stream, const Header& header) {
	// write magic sequence
	stream.write(Header::magic, 8);

	writeElement(stream, header.width);
	writeElement(stream, header.width);
	writeElement(stream, header.pf);
	writeElement(stream, header.dwtLevels);
}

void wlfImageWrite(const char* file, const cv::Mat& img, const WlfParams& params) {
	// convert input to ycbcr
	cv::Mat colorTransformed;
	cv::cvtColor(img, colorTransformed, CV_BGR2YCrCb);

	// TODO: chromatic subsampling

	// open file
	std::ofstream out(file, std::ios_base::binary);

	// write header
	Header header = { img.cols, img.rows, Header::PixelFormat::YCbCr444, params.dwtLevels };
	writeHeader(out, header);

	// convert input to one channel 32bit float
	cv::Mat image;
	colorTransformed.convertTo(image, CV_32FC3);
	image = image.reshape(1);

	// dwt with specified levels with cdf97 wavelet
	WaveletTransform wt(std::make_shared<Cdf97Wavelet>(), params.dwtLevels);
	wt.forward2d(image);

	// write dwt result
	for (int y = 0; y < image.rows; ++y) {
		for (int x = 0; x < image.cols; x++) {
			writeElement(out, image.at<float>(y, x));
		}
	}
}

template <typename T>
void readElement(std::istream& stream, T& elm) {
	stream.read(reinterpret_cast<char*>(&elm), sizeof(elm));
	if (!stream)
		throw std::runtime_error("Unable to read element from stream");
}

static Header readHeader(std::istream& stream) {
	char magicBuff[9] = {0};
	stream.read(magicBuff, 8);
	if (std::string(Header::magic) != magicBuff)
		throw std::runtime_error("Invalid magic number!");

	Header header;
	readElement(stream, header.width);
	readElement(stream, header.height);
	readElement(stream, header.pf);
	readElement(stream, header.dwtLevels);

	return header;
}

cv::Mat wlfImageRead(const char* file) {
	std::ifstream in(file, std::ios_base::binary);
	Header header = readHeader(in);

	// alloc buffer for dwt
	int channels = header.pf == Header::PixelFormat::Gray ? 1 : 3;
	size_t dwtBuffLen = header.width * header.height * channels * sizeof(float);
	std::vector<char> dwtBuff(dwtBuffLen);

	// read dwt
	in.read(dwtBuff.data(), dwtBuff.size());

	// create cv image with dwtBuff
	auto image = cv::Mat(header.height, header.width * channels, CV_32F);
	memcpy(image.data, dwtBuff.data(), dwtBuff.size());

	// dwt with specified levels with cdf97 wavelet
	WaveletTransform wt(std::make_shared<Cdf97Wavelet>(), header.dwtLevels);
	wt.inverse2d(image);

	// convert image to 8bit
	cv::Mat tmp;
	image.convertTo(tmp, CV_8U);
	image = tmp.reshape(channels);

	// transform color to bgr
	cv::Mat colorTransformed = image;
	colorTransforms[static_cast<int>(header.pf)](image, colorTransformed);

	return colorTransformed;
}
