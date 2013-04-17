/**
 * @file wavelettransform.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef WAVELET_TRANSFORM_H
#define WAVELET_TRANSFORM_H

#include "wavelet.h"

#include <memory>
#include <list>

typedef std::vector<double> VectorXd;

class WaveletTransform
{
public:
	WaveletTransform(std::shared_ptr<Wavelet> wavelet, int numLevels);
	~WaveletTransform();

	void forward1d(VectorXd& signal);

	void inverse1d(VectorXd& dwt);
private:
	std::shared_ptr<Wavelet> wavelet;
	int numLevels;
};

#endif // !WAVELET_TRANSFORM_H
