/**
 * @file wavelettransform.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef WAVELET_TRANSFORM_H
#define WAVELET_TRANSFORM_H

#include "mathutils.h"

#include <memory>
#include <list>

struct Wavelet
{
	VectorXd lowPassAnalysisFilter;
	VectorXd highPassAnalysisFilter;
	VectorXd lowPassSynthesisFilter;
	VectorXd highPassSynthesisFilter;
};

class WaveletTransform
{
public:
	WaveletTransform(Wavelet wavelet, int numLevels);
	~WaveletTransform();

	std::list<VectorXd> forward1d(const VectorXd& signal);

	VectorXd inverse1d(const std::list<VectorXd>& dwt);
private:
	static const int DOWNSAMP_FACTOR = 2;

	void forwardStep1d(const VectorXd& signal, VectorXd& approxCoefs, VectorXd& detailCoefs);
	void symmetricExtense(VectorXd& signal, size_t n);

	Wavelet wavelet;
	int numLevels;
};

#endif // !WAVELET_TRANSFORM_H
