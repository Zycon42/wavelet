/**
 * @file ezwdecoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef EZW_DECODER_H
#define EZW_DECODER_H

#include "ezw.h"
#include "bitstream.h"
#include "arithmdecoder.h"

#include <opencv2/core/core.hpp>

#include <memory>
#include <deque>
#include <vector>

/**
 * Embedded zero tree wavelet transform decoder.
 * @see http://www.polyvalens.com/blog/wavelets/ezw/
 */
class EzwDecoder : public EzwCodec
{
public:
	/**
	 * Constructs new decoder.
	 * @param adecoder dominant pass will be decoded by this arithmetic decoder
	 * @param bsr stream where subordinate pass is
	 */
	EzwDecoder(std::shared_ptr<ArithmeticDecoder>& adecoder, std::shared_ptr<BitStreamReader>& bsr) 
		: dataModel(4), adecoder(adecoder), bitStreamReader(bsr), pixels(0) { }

	/**
	 * Decodes matrix from streams.
	 * @param threshold threshold value used while encoding
	 * @param minThreshold minimum threshold value used while encoding
	 * @retval decoded matrix
	 */
	void decode(int32_t threshold, int32_t minThreshold, cv::Mat& mat);
private:
	void dominantPass(int32_t threshold, cv::Mat& mat);
	void subordinatePass(int32_t threshold, int32_t minThreshold, cv::Mat& mat);

	void initDominantPassQueue(int32_t threshold, cv::Mat& m);
	Element decodeElement(int32_t threshold, size_t x, size_t y, cv::Mat& m);
	Element::Code readElementCode();

	AdaptiveDataModel dataModel;
	std::shared_ptr<ArithmeticDecoder> adecoder;

	std::shared_ptr<BitStreamReader> bitStreamReader;

	std::deque<Element> dpQueue;

	struct ElementCoord
	{
		ElementCoord() { }
		ElementCoord(size_t x, size_t y) : x(x), y(y) { }
		size_t x, y;
	};
	std::vector<ElementCoord> subordVec;
	size_t pixels;
};

#endif // !EZW_DECODER_H
