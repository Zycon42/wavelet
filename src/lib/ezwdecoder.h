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

#include <opencv2/core/core.hpp>

#include <deque>
#include <vector>

class EzwDecoder : public EzwCodec
{
public:
	explicit EzwDecoder(BitStreamReader bsr) : bitStreamReader(std::move(bsr)), pixels(0) { }

	void decode(int32_t threshold, cv::Mat& mat);
private:
	void dominantPass(int32_t threshold, cv::Mat& mat);
	void subordinatePass(int32_t threshold, cv::Mat& mat);

	void initDominantPassQueue(int32_t threshold, cv::Mat& m);
	Element decodeElement(int32_t threshold, size_t x, size_t y, cv::Mat& m);
	Element::Code readElementCode();

	BitStreamReader bitStreamReader;

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
