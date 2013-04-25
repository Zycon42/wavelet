/**
 * @file wlfimage.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "wlfimage.h"

#include "wavelettransform.h"
#include "cdf97wavelet.h"
#include "cdf53wavelet.h"
#include "ezwencoder.h"
#include "ezwdecoder.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <functional>
#include <cassert>

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

static std::unique_ptr<WaveletTransform> createWaveletTransform(WlfImage::WaveletType type, int numlevels) {
	switch (type)
	{
	case WlfImage::WaveletType::Cdf97:
		return std::unique_ptr<WaveletTransform>(WaveletTransformFactory::create<Cdf97Wavelet>(numlevels));
	case WlfImage::WaveletType::Cdf53:
		return std::unique_ptr<WaveletTransform>(WaveletTransformFactory::create<Cdf53Wavelet>(numlevels));
	default:
		throw std::runtime_error("Unknown wavelet");
	}
}

struct Header
{
	static const char* MAGIC;
	static const size_t MAGIC_LEN = 8;

	uint32_t width;
	uint32_t height;
	WlfImage::PixelFormat::Type pf;
	uint8_t dwtLevels;
	WlfImage::WaveletType waveletType;
	uint16_t quantStep;
};

const char* Header::MAGIC = "\x89\x57\x4c\x46\x0d\x0a\x1a\x0a";

class ImageWriter
{
public:
	explicit ImageWriter(const char* file) : ofile(file, std::ios_base::binary) {
		if (!ofile)
			throw std::runtime_error("Unable to open file\"" + std::string(file) + "\" for writing!");
	}

	void writeHeader(const Header& header) {
		// write magic sequence
		ofile.write(Header::MAGIC, Header::MAGIC_LEN);

		writeElement(header.width);
		writeElement(header.height);
		writeElement(header.pf);
		writeElement(header.dwtLevels);
		writeElement(header.waveletType);
		writeElement(header.quantStep);
	}

	void writeChannel(cv::Mat& channel, size_t compressRate) {
		assert(channel.type() == CV_32S);

		auto threshold = EzwEncoder::computeInitTreshold(channel);
		writeElement(threshold);

		int32_t minTreshold = compressRate != 0 ? 1 << (compressRate - 1) : 0;
		writeElement(minTreshold);

		// create separate streams for dominant and subordinant ezw passes
		std::ostringstream dominantSS, subordSS;
		auto dominantBS = std::make_shared<BitStreamWriter>(&dominantSS);
		auto subordBS = std::make_shared<BitStreamWriter>(&subordSS);

		// ezw encode
		auto ezwEncoder = EzwEncoder(std::make_shared<ArithmeticEncoder>(dominantBS), subordBS);
		ezwEncoder.encode(channel, threshold, minTreshold);

		// write passes to file
		auto dominantEncoded = dominantSS.str();
		auto subordEncoded = subordSS.str();
		writeElement(dominantEncoded.size());
		writeElement(subordEncoded.size());
		ofile.write(dominantEncoded.data(), dominantEncoded.size());
		ofile.write(subordEncoded.data(), subordEncoded.size());
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

template <typename T>
int signum(T val) {
	return (T(0) < val) - (val < T(0));
}

static cv::Mat scalarQuantize(const cv::Mat& m, int step) {
	cv::Mat result(m.size(), CV_32S);
	if (m.type() == CV_32F) {
		for (int y = 0; y < m.rows; ++y) {
			for (int x = 0; x < m.cols; ++x) {
				float val = m.at<float>(y, x);
				result.at<int32_t>(y, x) = static_cast<int32_t>(signum(val) * floor(abs(val) / step + 0.5));
			}
		}
	} else if (m.type() == CV_32S) {
		for (int y = 0; y < m.rows; ++y) {
			for (int x = 0; x < m.cols; ++x) {
				int32_t val = m.at<int32_t>(y, x);
				result.at<int32_t>(y, x) = static_cast<int32_t>(signum(val) * floor(abs(val) / step + 0.5));
			}
		}
	} else
		throw std::runtime_error("scalarQuantize: invalid matrix type");

	return result;
}

void WlfImage::save(const char* file, const cv::Mat& img, const Params& params /* = Params */) {
	// open file
	ImageWriter writer(file);

	// write header
	Header header = { img.cols, img.rows, params.pf, params.dwtLevels, params.waveletType, params.quantizationStep };
	writer.writeHeader(header);

	// transform input from bgr to desired color model
	cv::Mat colorTransformed;
	PixelFormat::transformTo(params.pf, img, colorTransformed);

	// dwt with specified levels
	auto wt = createWaveletTransform(params.waveletType, params.dwtLevels);

	// convert input to one channel 32bit float
	cv::Mat image;
	colorTransformed.convertTo(image, wt->getType());

	// split image channels
	std::vector<cv::Mat> channels;
	cv::split(image, channels);
	assert(channels.size() == 3);

	// TODO: chromatic subsampling

	// dwt channels and write it
	for (auto& channel : channels) {
		wt->forward2d(channel);
		writer.writeChannel(scalarQuantize(channel, params.quantizationStep), params.compressRate);
	}
}

class ImageReader
{
public:
	ImageReader(const char* file) : ifile(file, std::ios_base::binary) {
		if (!ifile)
			throw std::runtime_error("Unable to open file\"" + std::string(file) + "\" for reading!");
	}

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
		readElement(header.waveletType);
		readElement(header.quantStep);

		return header;
	}

	cv::Mat readChannel(size_t width, size_t height) {
		int32_t threshold;
		readElement(threshold);
		int32_t minTreshold;
		readElement(minTreshold);

		size_t dominantSize, subordSize;
		readElement(dominantSize);
		readElement(subordSize);

		std::unique_ptr<char[]> dominantPass(new char[dominantSize]);
		std::unique_ptr<char[]> subordPass(new char[subordSize]);
		ifile.read(dominantPass.get(), dominantSize);
		ifile.read(subordPass.get(), subordSize);

		std::istringstream dominantSS(std::string(dominantPass.get(), dominantSize));
		std::istringstream subordSS(std::string(subordPass.get(), subordSize));
		auto dominantBS = std::make_shared<BitStreamReader>(&dominantSS);
		auto subordBS = std::make_shared<BitStreamReader>(&subordSS);

		cv::Mat result = cv::Mat::zeros(width, height, CV_32S);
		auto ezwDecoder = EzwDecoder(std::make_shared<ArithmeticDecoder>(dominantBS), subordBS);
		ezwDecoder.decode(threshold, minTreshold, result);

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

static cv::Mat dequantize(const cv::Mat& m, int step) {
	return m.mul(cv::Scalar::all(step));
}

cv::Mat WlfImage::read(const char* file) {
	ImageReader reader(file);
	Header header = reader.readHeader();

	// read channels
	int numChannels = header.pf == PixelFormat::Type::Gray ? 1 : 3;
	std::vector<cv::Mat> channels(numChannels);
	auto wt = createWaveletTransform(header.waveletType, header.dwtLevels);
	for (int i = 0; i < numChannels; ++i) {
		// read channel and perform idwt
		auto channel = dequantize(reader.readChannel(header.width, header.height), header.quantStep);

		cv::Mat convertedChannel;
		channel.convertTo(convertedChannel, wt->getType());
		wt->inverse2d(convertedChannel);

		// convert result to 8bit
		convertedChannel.convertTo(channels[i], CV_8U);
	}

	// merge channels to one image
	cv::Mat image;
	cv::merge(channels, image);

	// transform color from image color model to bgr
	cv::Mat colorTransformed = image;
	PixelFormat::transformFrom(header.pf, image, colorTransformed);

	return colorTransformed;
}
