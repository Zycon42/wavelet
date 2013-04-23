/**
 * @file ezwencoder.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "ezwencoder.h"

#include <stdexcept>
#include <cstdint>
#include <cmath>

void EzwEncoder::encode(cv::Mat& mat, int& initTreshold) {
	if (mat.type() != CV_32S)
		throw std::runtime_error("EzwEncoder::encode can operate only on 32b integer matrices");

	initTreshold = computeInitTreshold(mat);
	int32_t threshold = initTreshold;

	do {
		dominantPass(mat, threshold);
		subordinatePass(threshold);
		threshold >>= 1;		// shift to right by one means divide by two
	} while (threshold > 0);	// TODO: this is lossless, make this changeable to allow lossy compression

	bitStreamWriter.flush();
}

int32_t EzwEncoder::computeInitTreshold(const cv::Mat& m) {
	// find max of absolute values in m
	double absmax;
	cv::minMaxIdx(cv::abs(m), nullptr, &absmax);

	// return 2^(floor(log_2(absmax)))
	return 1 << static_cast<int32_t>(floor(log10(absmax) / log10(2.0)));
}

void EzwEncoder::dominantPass(cv::Mat& mat, int32_t threshold) {
	initDominantPassQueue(mat, threshold);

	do {
		// get elm from queue and output it
		auto elm = dpQueue.front();
		dpQueue.pop_front();
		outputCode(elm.code);

		// we don't need to code zerotree children cos they are zero
		if (elm.code != Element::Code::ZeroTreeRoot) {
			// handle elm children
			auto minx = elm.x * 2;
			auto miny = elm.y * 2;
			auto maxx = minx + 1;
			auto maxy = miny + 1;
			if (maxx <= (size_t)mat.cols && maxy <= (size_t)mat.rows) {						// last level doesn't have children
				for (auto y = miny; y < maxy + 1; ++y) {
					for (auto x = minx; x < maxx + 1; ++x) {
						auto child = codeElement(mat, x, y, threshold);
						dpQueue.push_back(child);
					}
				}
			}
		}

	} while(!dpQueue.empty());
}

void EzwEncoder::subordinatePass(int32_t threshold) {
	threshold >>= 1;					// divide threshold by two
	if (threshold <= 0)
		return;

	for (auto elm : subordList) {
		// threshold is some power of two so it has single bit set
		// and since we are lowering thresholds from max value we
		// can determine if elm is higher than threshold just by simple
		// test if elm has same bit set like threshold
		if ((elm & threshold) != 0) {
			bitStreamWriter.writeBit(true);
		} else {
			bitStreamWriter.writeBit(false);
		}
	}
}

void EzwEncoder::initDominantPassQueue(cv::Mat& m, int32_t threshold) {
	Element elm;

	elm = codeElement(m, 0, 0, threshold);
	outputCode(elm.code);

	elm = codeElement(m, 1, 0, threshold);
	dpQueue.push_back(elm);

	elm = codeElement(m, 0, 1, threshold);
	dpQueue.push_back(elm);

	elm = codeElement(m, 1, 1, threshold);
	dpQueue.push_back(elm);
}

EzwCodec::Element EzwEncoder::codeElement(cv::Mat& m, size_t x, size_t y, int32_t threshold) {
	Element result(x, y);
	result.code = computeElementCode(m, x, y, threshold);
	if (result.code == Element::Code::Pos || result.code == Element::Code::Neg) {
		subordList.push_back(abs(m.at<int32_t>(y, x)));
		m.at<int32_t>(y, x) = 0;
	}

	return result;
}

EzwCodec::Element::Code EzwEncoder::computeElementCode(cv::Mat& m, size_t x, size_t y, int32_t threshold) {
	auto coef = m.at<int32_t>(y, x);
	if (abs(coef) >= threshold) {
		if (coef >= 0)
			return Element::Code::Pos;
		else
			return Element::Code::Neg;
	} else {
		if (isZerotreeRoot(m, x, y, threshold))
			return Element::Code::ZeroTreeRoot;
		else
			return Element::Code::IsolatedZero;
	}
}

bool EzwEncoder::isZerotreeRoot(cv::Mat& m, size_t x, size_t y, int32_t threshold) {
	// handle first coef
	if (x == 0 && y == 0) {
		// find max value in rest of m
		auto tmp = m.at<int32_t>(y, x);
		m.at<int32_t>(y, x) = std::numeric_limits<int32_t>::min();
		double absmax;
		cv::minMaxIdx(cv::abs(m), nullptr, &absmax);
		m.at<int32_t>(y, x) = tmp;

		// if [0,0] is not highest value then its not zerotree root
		if (absmax >= threshold)
			return false;
		return true;
	}

	auto minx = x * 2;
	auto miny = y * 2;
	auto maxx = (x + 1) * 2;
	auto maxy = (y + 1) * 2;
	if (minx == m.cols || miny == m.rows)
		return true;

	while (maxx <= (size_t)m.cols && maxy <= (size_t)m.rows) {
		for (auto i = miny; i < maxy; ++i) {
			for (auto j = minx; j < maxx; ++j) {
				if (abs(m.at<int32_t>(i, j)) >= threshold)
					return false;
			}
		}

		minx *= 2;
		miny *= 2;
		maxx *= 2;
		maxy *= 2;
	}

	return true;
}

void EzwEncoder::outputCode(Element::Code code) {
	switch (code)
	{
	case EzwCodec::Element::Code::Pos:
		bitStreamWriter.writeBit(false);
		bitStreamWriter.writeBit(true);
		break;
	case EzwCodec::Element::Code::Neg:
		bitStreamWriter.writeBit(true);
		bitStreamWriter.writeBit(true);
		break;
	case EzwCodec::Element::Code::IsolatedZero:
		bitStreamWriter.writeBit(true);
		bitStreamWriter.writeBit(false);
		break;
	case EzwCodec::Element::Code::ZeroTreeRoot:
		bitStreamWriter.writeBit(false);
		bitStreamWriter.writeBit(false);
		break;
	default:
		break;
	}
}