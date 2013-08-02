/**
 * @file spihtencoder.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "spihtencoder.h"

#include "utils.h"

int SpihtEncoder::computeMaxSteps(const cv::Mat& m) {
	// find max of absolute values in m
	double absmax;
	cv::minMaxIdx(cv::abs(m), nullptr, &absmax);

	return floor(log(absmax) / log(2.0));
}

void SpihtEncoder::initialize(cv::Mat& img, int numlevels) {
	// clear lists
	lip.clear();
	lsp.clear();
	lis.clear();

	this->numlevels = numlevels;
	this->img = &img;

	// add the coordinates in H(highest pyramid level) to the LIP
	// and only those with descendants also to the LIS as type A
	for (int y = 0; y < img.rows / (1 << numlevels); y++) {
		for (int x = 0; x < img.cols / (1 << numlevels); x++) {
			lip.push_back(cv::Point(x, y));
			if ((x % 2 != 0) || (y % 2 != 0))
				lis.push_back(SetItem(x, y, SetItem::Type::A));
		}
	}
}

void SpihtEncoder::encode(cv::Mat& img, int numlevels, int steps) {
	if (img.type() != CV_32S)
		throw std::runtime_error("SpihtEncoder::encode can operate only on 32b integer matrices");

	initialize(img, numlevels);

	while (steps >= 0) {

		sortingPass(steps);

		refinementPass(steps);

		// update quantization step
		steps--;
	}
}

void SpihtEncoder::sortingPass(int step) {

	for (size_t i = 0; i < lip.size(); i++) {
		bool significant = isPixelSignificant(lip[i], step);
		bitStreamWriter->writeBit(significant);

		if (significant) {
			// write sign of current pixel value
			bitStreamWriter->writeBit(sign(img->at<int32_t>(lip[i])));

			// move lip[i] to lsp
			lsp.push_back(lip[i]);
			lip.erase(lip.begin() + i);
			i--;
		}
	}

	for (size_t i = 0; i < lis.size(); i++) {
		// type A
		if (lis[i].type == SetItem::Type::A) {
			bool significant = isDescendantSetSignificant(lis[i].point, step);
			bitStreamWriter->writeBit(significant);

			if (significant) {
				cv::Point off = getOffspring(lis[i].point);
				processOffspring(off.x, off.y, step);
				processOffspring(off.x + 1, off.y, step);
				processOffspring(off.x, off.y + 1, step);
				processOffspring(off.x + 1, off.y + 1, step);

				// test if L(lis[i].point) is not empty
				if (getOffspring(off).x != -1)
					lis.emplace_back(lis[i].point, SetItem::Type::B);
				lis.erase(lis.begin() + i);
				i--;
			}
		// type B
		} else {
			bool significant = isIndirectDescendantSetSignificant(lis[i].point, step);
			bitStreamWriter->writeBit(significant);

			if (significant) {
				// add every offspring to lis as type A
				cv::Point off = getOffspring(lis[i].point);
				lis.emplace_back(cv::Point(off.x, off.y), SetItem::Type::A);
				lis.emplace_back(cv::Point(off.x + 1, off.y), SetItem::Type::A);
				lis.emplace_back(cv::Point(off.x, off.y + 1), SetItem::Type::A);
				lis.emplace_back(cv::Point(off.x + 1, off.y + 1), SetItem::Type::A);

				// remove current item from lis
				lis.erase(lis.begin() + i);
				i--;
			}
		}
	}
}

void SpihtEncoder::refinementPass(int step) {
	// for each entry in lsp
	for (size_t i = 0; i < lsp.size(); i++) {
		auto value = img->at<int32_t>(lsp[i]);
		// except those included in the last sorting pass
		if (abs(value) >= (1 << (step + 1))) {
			// write n-th bit of coefficient
			bitStreamWriter->writeBit(value & (1 << step));
		}
	}
}

void SpihtEncoder::processOffspring(int x, int y, int step) {
	bool significant = isPixelSignificant(cv::Point(x, y), step);
	bitStreamWriter->writeBit(significant);

	if (significant) {
		lsp.emplace_back(x, y);
		bitStreamWriter->writeBit(sign(img->at<int32_t>(y, x)));
	} else
		lip.emplace_back(x, y);
}

bool SpihtEncoder::isPixelSignificant(const cv::Point& coords, int step) {
	return abs(img->at<int32_t>(coords)) >= (1 << step);
}

cv::Point SpihtEncoder::getOffspring(int x, int y) {
	// get size of highest pyramid level
	int hx = img->cols / (1 << numlevels);
	int hy = img->rows / (1 << numlevels);

	cv::Point ret;

	// if we are on highest pyramid level
	if (x < hx && y < hy) {
		// pixels in left up corner of 2x2 group doesn't have any offspring
		ret = cv::Point(x, y);
		if (x % 2 == 1)
			ret.x = x + hx - 1;
		if (y % 2 == 1)
			ret.y = y + hy - 1;
		if (ret.x == x && ret.y == y)
			ret = cv::Point(-1, -1);
	} else {
		ret = cv::Point(2 * x, 2 * y);
		if (ret.x >= img->cols || ret.y >= img->rows)
			ret = cv::Point(-1, -1);
	}

	return ret;
}
