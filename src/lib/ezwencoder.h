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
#include "arithmencoder.h"

#include <opencv2/core/core.hpp>

#include <memory>
#include <deque>
#include <list>

class EzwEncoder : public EzwCodec
{
public:
	EzwEncoder(std::shared_ptr<ArithmeticEncoder>& aencoder, std::shared_ptr<BitStreamWriter>& bsw) 
		: dataModel(4), aencoder(aencoder), bitStreamWriter(bsw) { }

	void encode(cv::Mat& mat, int32_t threshold, int32_t minThreshold = 0);

	static int32_t computeInitTreshold(const cv::Mat& m);
private:
	void dominantPass(cv::Mat& mat, int32_t threshold);
	void subordinatePass(int32_t threshold, int32_t minThreshold);

	void initDominantPassQueue(cv::Mat& m, int32_t threshold);
	Element codeElement(cv::Mat& m, size_t x, size_t y, int32_t threshold);
	Element::Code computeElementCode(cv::Mat& m, size_t x, size_t y, int32_t threshold);
	bool isZerotreeRoot(cv::Mat& m, size_t x, size_t y, int32_t threshold);
	void outputCode(Element::Code code);

	AdaptiveDataModel dataModel;
	std::shared_ptr<ArithmeticEncoder> aencoder;
	std::shared_ptr<BitStreamWriter> bitStreamWriter;

	std::deque<Element> dpQueue;
	std::list<int32_t> subordList;
};

#endif // !EZW_ENCODER_H
