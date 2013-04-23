/**
 * @file ezwdecoder.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "ezwdecoder.h"

//#define DUMP_RES

void EzwDecoder::decode(int32_t threshold, cv::Mat& mat) {
	if (mat.type() != CV_32S)
		throw std::runtime_error("EzwDecoder::decode can operate only on 32b integer matrices");

	do {
		dominantPass(threshold, mat);

		subordinatePass(threshold, mat);

		threshold >>= 1;
	} while(threshold > 0);

#ifdef DUMP_RES
	std::cerr << std::endl;
#endif
}

void EzwDecoder::dominantPass(int32_t threshold, cv::Mat& mat) {
	initDominantPassQueue(threshold, mat);

	do {
		auto elm = dpQueue.front();
		dpQueue.pop_front();
		if (elm.code == Element::Code::Neg || elm.code == Element::Code::Pos)
			pixels++;

		if (elm.code != Element::Code::ZeroTreeRoot) {
			// handle elm children
			auto minx = elm.x * 2;
			auto miny = elm.y * 2;
			auto maxx = minx + 1;
			auto maxy = miny + 1;
			if (maxx <= (size_t)mat.cols && maxy <= (size_t)mat.rows) {						// last level doesn't have children
				for (auto y = miny; y < maxy + 1; ++y) {
					for (auto x = minx; x < maxx + 1; ++x) {
						auto child = decodeElement(threshold, x, y, mat);
						dpQueue.push_back(child);
					}
				}
			}
		}

	} while (!dpQueue.empty());
}

void EzwDecoder::subordinatePass(int32_t threshold, cv::Mat& mat) {
	threshold >>= 1;
	if (threshold <= 0)
		return;

	for (size_t i = 0; i < pixels; ++i) {
		auto coord = subordVec[i];
		auto elm = mat.at<int32_t>(coord.y, coord.x);
		if (bitStreamReader->readBit()) {
#ifdef DUMP_RES
			std::cerr << "1";
#endif
			if (elm < 0)
				mat.at<int32_t>(coord.y, coord.x) = elm - threshold;
			else
				mat.at<int32_t>(coord.y, coord.x) = elm + threshold;
		} 
#ifdef DUMP_RES
		else
			std::cerr << "0";
#endif
	}
}

void EzwDecoder::initDominantPassQueue(int32_t threshold, cv::Mat& m) {
	Element elm;

	elm = decodeElement(threshold, 0, 0, m);
	if (elm.code == Element::Code::Neg || elm.code == Element::Code::Pos)
		pixels++;

	elm = decodeElement(threshold, 1, 0, m);
	dpQueue.push_back(elm);

	elm = decodeElement(threshold, 0, 1, m);
	dpQueue.push_back(elm);

	elm = decodeElement(threshold, 1, 1, m);
	dpQueue.push_back(elm);
}

EzwCodec::Element EzwDecoder::decodeElement(int32_t threshold, size_t x, size_t y, cv::Mat& m) {
	Element result(x, y);
	result.code = readElementCode();

	if (result.code == Element::Code::Pos) {
		m.at<int32_t>(y, x) = threshold;
		subordVec.push_back(ElementCoord(x, y));
	} else if (result.code == Element::Code::Neg) {
		m.at<int32_t>(y, x) = -threshold;
		subordVec.push_back(ElementCoord(x, y));
	}

	return result;
}

EzwCodec::Element::Code EzwDecoder::readElementCode() {
	auto code = static_cast<Element::Code>(adecoder->decode(&dataModel));
#ifdef DUMP_RES
	switch (code)
	{
	case EzwCodec::Element::Code::Pos:
		std::cerr << "p";
		break;
	case EzwCodec::Element::Code::Neg:
		std::cerr << "n";
		break;
	case EzwCodec::Element::Code::IsolatedZero:
		std::cerr << "z";
		break;
	case EzwCodec::Element::Code::ZeroTreeRoot:
		std::cerr << "t";
		break;
	default:
		break;
	}
#endif
	return code;
/*	if (adecoder->reader()->readBit()) {
		if (adecoder->reader()->readBit()) {
#ifdef DUMP_RES
			std::cerr << "t";
#endif
			return EzwCodec::Element::Code::ZeroTreeRoot;
		} else {
#ifdef DUMP_RES
			std::cerr << "z";
#endif
			return EzwCodec::Element::Code::IsolatedZero;
		}
	} else {
		if (adecoder->reader()->readBit()) {
#ifdef DUMP_RES
			std::cerr << "n";
#endif
			return EzwCodec::Element::Code::Neg;
		} else {
#ifdef DUMP_RES
			std::cerr << "p";
#endif
			return EzwCodec::Element::Code::Pos;
		}
	}*/
}