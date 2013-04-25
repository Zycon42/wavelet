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

/**
 * Embedded zero tree wavelet transform encoder.
 * This transform could be lossy if you pass non-zero
 * minThreshold param to encode method, the higher minThreshold
 * is the more information you lose but wavelet coefs can
 * still be reconstructed.
 * @see http://www.polyvalens.com/blog/wavelets/ezw/
 */
class EzwEncoder : public EzwCodec
{
public:
	/**
	 * Constructs new encoder.
	 * @param aencoder arithmetic encoder used for dominant pass results
	 * @param bsw stream for subordinate pass results
	 */
	EzwEncoder(std::shared_ptr<ArithmeticEncoder>& aencoder, std::shared_ptr<BitStreamWriter>& bsw) 
		: dataModel(4), aencoder(aencoder), bitStreamWriter(bsw) { }

	/**
	 * Encodes matrix to streams.
	 * @param mat input matrix to be encoded
	 * @param threshold initial threshold should be from computeInitTreshold call and power of two
	 * @param minThreshold threshold when encoding stops, should be power of two.
	 */
	void encode(cv::Mat& mat, int32_t threshold, int32_t minThreshold = 0);

	/// Computes initial threshold for encoding matrix m.
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
