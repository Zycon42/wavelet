/**
 * @file ezwencoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef EZW_ENCODER_H
#define EZW_ENCODER_H

#include "ezw.h"
#include "bitstream.h"

#include <opencv2/core/core.hpp>

#include <deque>
#include <list>

class EzwEncoder : public EzwCodec
{
public:
	explicit EzwEncoder(BitStreamWriter bsw) : bitStreamWriter(std::move(bsw)) { }

	void encode(cv::Mat& mat, int32_t threshold);

	static int32_t computeInitTreshold(const cv::Mat& m);
private:
	void dominantPass(cv::Mat& mat, int32_t threshold);
	void subordinatePass(int32_t threshold);

	void initDominantPassQueue(cv::Mat& m, int32_t threshold);
	Element codeElement(cv::Mat& m, size_t x, size_t y, int32_t threshold);
	Element::Code computeElementCode(cv::Mat& m, size_t x, size_t y, int32_t threshold);
	bool isZerotreeRoot(cv::Mat& m, size_t x, size_t y, int32_t threshold);
	void outputCode(Element::Code code);

	BitStreamWriter bitStreamWriter;

	std::deque<Element> dpQueue;
	std::list<int32_t> subordList;
};

#endif // !EZW_ENCODER_H
