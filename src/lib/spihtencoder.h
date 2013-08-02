/**
 * @file spihtencoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef SPIHT_ENCODER_H
#define SPIHT_ENCODER_H

#include "bitstream.h"

#include <opencv2/core/core.hpp>

#include <memory>
#include <vector>

/**
 * @see http://www.cipr.rpi.edu/research/SPIHT/EW_Code/csvt96_sp.pdf
 */
class SpihtEncoder
{
public:
	explicit SpihtEncoder(std::shared_ptr<BitStreamWriter>& bsw) : bitStreamWriter(bsw), numlevels(0), img(nullptr) { }

	void encode(cv::Mat& img, int numlevels, int steps);

	int computeMaxSteps(const cv::Mat& m);
private:
	struct SetItem
	{
		enum class Type { A, B };

		cv::Point point;
		Type type;

		SetItem() { }
		SetItem(const cv::Point& p, Type type) : point(p), type(type) { }
		SetItem(int x, int y, Type type) : point(x, y), type(type) { }
	};

	void initialize(cv::Mat& img, int numlevels);

	void sortingPass(int step);

	void refinementPass(int step);

	void processOffspring(int x, int y, int step);

	/**
	 * Get first offspring of given pixel.
	 * You can obtain another 3 offspring by adding 1 to each coordinate.
	 * @param x pixel x coord
	 * @param y pixel y coord
	 * @return first offspring or (-1, -1) when there are no offspring at all.
	 */
	cv::Point getOffspring(int x, int y);

	cv::Point getOffspring(const cv::Point& p) {
		return getOffspring(p.x, p.y);
	}

	bool isPixelSignificant(const cv::Point& coords, int step);

	template <int StartDepth>
	bool isSetSignificant(const cv::Point& coords, int step, int depth);

	bool isDescendantSetSignificant(const cv::Point& coords, int step) {
		return isSetSignificant<1>(coords, step, 0);
	}

	bool isIndirectDescendantSetSignificant(const cv::Point& coords, int step) {
		return isSetSignificant<2>(coords, step, 0);
	}

	std::shared_ptr<BitStreamWriter> bitStreamWriter;

	std::vector<cv::Point> lsp;		/// list of significant pixels
	std::vector<cv::Point> lip;		/// list of insignificant pixels
	std::vector<SetItem> lis;		/// list of insignificant sets

	int numlevels;
	cv::Mat* img;
};

template <int StartDepth>
bool SpihtEncoder::isSetSignificant(const cv::Point& coords, int step, int depth) {
	if (depth >= StartDepth && isPixelSignificant(coords, step))
		return true;

	auto offspring = getOffspring(coords);
	if (offspring.x == -1 || offspring.y == -1)
		return false;

	if (isSetSignificant(coords, step, depth + 1))
		return true;
	else if (isSetSignificant(cv::Point(coords.x + 1, coords.y), step, depth + 1))
		return true;
	else if (isSetSignificant(cv::Point(coords.x, coords.y + 1), step, depth + 1))
		return true;
	else if (isSetSignificant(cv::Point(coords.x + 1, coords.y + 1), step, depth + 1))
		return true;

	return false;
}

#endif // !SPIHT_ENCODER_H
