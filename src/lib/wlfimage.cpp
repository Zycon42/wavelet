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
#include <stdexcept>
#include <string>
#include <vector>
#include <functional>

void WlfImage::PixelFormat::transformFrom(Type type, const cv::Mat& src, cv::Mat& dest) {
	using namespace std::placeholders;
	static std::function<void (const cv::Mat&, cv::Mat&)> colorTransforms[] = {
		std::bind(cv::cvtColor, _1, _2, CV_RGB2BGR, 0), std::bind(cv::cvtColor, _1, _2, CV_GRAY2BGR, 0),
		std::bind(cv::cvtColor, _1, _2, CV_YCrCb2BGR, 0)
	};

	colorTransforms[static_cast<int>(type)](src, dest);
}

void WlfImage::PixelFormat::transformTo(Type type, const cv::Mat& src, cv::Mat& dest) {
	using namespace std::placeholders;
	static std::function<void (const cv::Mat&, cv::Mat&)> colorTransforms[] = {
		std::bind(cv::cvtColor, _1, _2, CV_BGR2RGB, 0), std::bind(cv::cvtColor, _1, _2, CV_BGR2GRAY, 0),
		std::bind(cv::cvtColor, _1, _2, CV_BGR2YCrCb, 0)
	};

	colorTransforms[static_cast<int>(type)](src, dest);
}

struct Header
{
	static const char* MAGIC;
	static const size_t MAGIC_LEN = 8;

	uint32_t width;
	uint32_t height;
	WlfImage::PixelFormat::Type pf;
	uint8_t dwtLevels;
};

const char* Header::MAGIC = "\x89\x57\x4c\x46\x0d\x0a\x1a\x0a";

class ImageWriter
{
public:
	explicit ImageWriter(const char* file) : ofile(file, std::ios_base::binary) {
	}

	void writeHeader(const Header& header) {
		// write magic sequence
		ofile.write(Header::MAGIC, Header::MAGIC_LEN);

		writeElement(header.width);
		writeElement(header.height);
		writeElement(header.pf);
		writeElement(header.dwtLevels);
	}

	void writeChannel(const cv::Mat& channel) {
		for (int y = 0; y < channel.rows; ++y) {
			for (int x = 0; x < channel.cols; x++) {
				ofile.write((const char*)channel.data + channel.step * y + channel.elemSize() * x, channel.elemSize());
			}
		}
	}
private:
	template <typename T>
	void writeElement(const T& elm) {
		ofile.write(reinterpret_cast<const char*>(&elm), sizeof(elm));
		if (!ofile)
			throw std::runtime_error("Unable to write element to stream");
	}

	std::ofstream ofile;
};

void WlfImage::save(const char* file, const cv::Mat& img, const Params& params /* = Params */) {
	// open file
	ImageWriter writer(file);

	// write header
	Header header = { img.cols, img.rows, params.pf, params.dwtLevels };
	writer.writeHeader(header);

	// transform input from bgr to desired color model
	cv::Mat colorTransformed;
	PixelFormat::transformTo(params.pf, img, colorTransformed);

	// convert input to one channel 32bit float
	cv::Mat image;
	colorTransformed.convertTo(image, CV_32FC3);

	// split image channels
	std::vector<cv::Mat> channels;
	cv::split(image, channels);
	assert(channels.size() == 3);

	// TODO: chromatic subsampling

	// dwt with specified levels with cdf97 wavelet
	WaveletTransform wt(std::make_shared<Cdf97Wavelet>(), params.dwtLevels);
	// handle channel
	for (auto channel : channels) {
		wt.forward2d(channel);
		writer.writeChannel(channel);
	}
}

class ImageReader
{
public:
	ImageReader(const char* file) : ifile(file, std::ios_base::binary) { }

	Header readHeader() {
		char magicBuff[Header::MAGIC_LEN + 1] = {0};	// +1 for trailing zero
		ifile.read(magicBuff, Header::MAGIC_LEN);
		if (std::string(Header::MAGIC) != magicBuff)
			throw std::runtime_error("Invalid magic number!");

		Header header;
		readElement(header.width);
		readElement(header.height);
		readElement(header.pf);
		readElement(header.dwtLevels);

		return header;
	}

	cv::Mat readChannel(size_t width, size_t height) {
		// create buffer for channel data storage
		size_t buffLen = width * height * sizeof(float);
		std::vector<char> buff(buffLen);

		// read channel data to buffer
		ifile.read(buff.data(), buff.size());

		// create resulting cv::Mat and copy channel data to it
		auto result = cv::Mat(width, height, CV_32F);
		memcpy(result.data, buff.data(), buff.size());

		return result;
	}
private:
	template <typename T>
	void readElement(T& elm) {
		ifile.read(reinterpret_cast<char*>(&elm), sizeof(elm));
		if (!ifile)
			throw std::runtime_error("Unable to read element from stream");
	}

	std::ifstream ifile;
};

cv::Mat WlfImage::read(const char* file) {
	ImageReader reader(file);
	Header header = reader.readHeader();

	// read channels
	int numChannels = header.pf == PixelFormat::Type::Gray ? 1 : 3;
	std::vector<cv::Mat> channels(numChannels);
	WaveletTransform wt(std::make_shared<Cdf97Wavelet>(), header.dwtLevels);
	for (int i = 0; i < numChannels; ++i) {
		// read channel and perform idwt
		auto channel = reader.readChannel(header.width, header.height);
		wt.inverse2d(channel);

		// convert result to 8bit
		channel.convertTo(channels[i], CV_8U);
	}

	// merge channels to one image
	cv::Mat image;
	cv::merge(channels, image);

	// transform color from image color model to bgr
	cv::Mat colorTransformed = image;
	PixelFormat::transformFrom(header.pf, image, colorTransformed);

	return colorTransformed;
}
