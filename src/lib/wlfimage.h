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

/**
 * Image of WaveLet Format.
 */
class WlfImage
{
public:
	/// Wavelets that can be used
	enum class WaveletType : uint8_t { Cdf97, Cdf53 };

	/// Format of pixel
	struct PixelFormat
	{
		/// Supported types
		enum class Type : uint8_t {
			RGB, Gray, YCbCr444
		};

		static void transformTo(Type, const cv::Mat&, cv::Mat&);
		static void transformFrom(Type, const cv::Mat&, cv::Mat&);
	};

	/// Wlf format parameters
	struct Params
	{
		Params() : pf(PixelFormat::Type::YCbCr444), dwtLevels(2),
			compressRate(0), quantizationStep(1), waveletType(WlfImage::WaveletType::Cdf97) { }

		PixelFormat::Type pf;	/// pixel format
		int dwtLevels;			/// num of dwt levels
		size_t compressRate;	/// number of least significant bits that won't be encoded
		int quantizationStep;	/// scalar quantization step
		WaveletType waveletType;/// wavelet to be used
	};

	/** 
	 * Read file in wlf format to OpenCV matrix.
	 * @param file path
	 * @return OpenCV matrix with 8bits per pixel and BGR color format
	 * @throws std::runtime_error when reading failed
	 */
	static cv::Mat read(const char* file);

	/**
	 * Saves OpenCV matrix to file in wlf format.
	 * @param file path to output file
	 * @param img OpenCV matrix with 8bits per pixel and BGR color format
	 * @param params wlf format parameter
	 * @throws std::runtime_error when saving failed
	 */
	static void save(const char* file, const cv::Mat& img, const Params& params = Params());
};

#endif // !WLF_IMAGE_H
