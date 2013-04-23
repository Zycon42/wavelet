/**
 * @file wlfimage.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef WLF_IMAGE_H
#define WLF_IMAGE_H

#include <opencv2/core/core.hpp>

#include <cstdint>

class WlfImage
{
public:
	struct PixelFormat
	{
		enum class Type : uint8_t {
			RGB, Gray, YCbCr444
		};

		static void transformTo(Type, const cv::Mat&, cv::Mat&);
		static void transformFrom(Type, const cv::Mat&, cv::Mat&);
	};

	struct Params
	{
		Params() : pf(PixelFormat::Type::YCbCr444), dwtLevels(2), compressRate(0) { }

		PixelFormat::Type pf;	/// pixel format
		int dwtLevels;			/// num of dwt levels
		size_t compressRate;	/// number of least significant bits that won't be encoded
	};

	static cv::Mat read(const char* file);

	static void save(const char* file, const cv::Mat& img, const Params& params = Params());
};

#endif // !WLF_IMAGE_H
