/**
 * @file wavelettransform.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef WAVELET_TRANSFORM_H
#define WAVELET_TRANSFORM_H

#include "wavelet.h"

#include <opencv2/core/core.hpp>

#include <memory>
#include <list>

typedef std::vector<float> VectorXf;

class WaveletTransform
{
public:
	WaveletTransform(std::shared_ptr<Wavelet> wavelet, int numLevels);
	~WaveletTransform();

	void forward1d(VectorXf& signal);

	void inverse1d(VectorXf& dwt);

	void forward2d(cv::Mat& signal);

	void inverse2d(cv::Mat& dwt);
private:
	std::shared_ptr<Wavelet> wavelet;
	int numLevels;
};

#endif // !WAVELET_TRANSFORM_H
