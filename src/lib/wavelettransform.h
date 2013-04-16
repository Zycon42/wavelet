/**
 * @file wavelettransform.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef WAVELET_TRANSFORM_H
#define WAVELET_TRANSFORM_H

#include "mathutils.h"

#include <list>
#include <map>

struct Wavelet
{
	Wavelet() {}
	Wavelet(VectorXd lpa, VectorXd hpa, VectorXd lps, VectorXd hps) :
		lowPassAnalysisFilter(std::move(lpa)), highPassAnalysisFilter(std::move(hpa)),
		lowPassSynthesisFilter(std::move(lps)), highPassSynthesisFilter(std::move(hps))
	{}

	VectorXd lowPassAnalysisFilter;
	VectorXd highPassAnalysisFilter;
	VectorXd lowPassSynthesisFilter;
	VectorXd highPassSynthesisFilter;
};

class WaveletFactory
{
public:
	enum class Type { Harr, Cdf97 };
	static Wavelet create(Type type);
private:
	static std::map<Type, Wavelet> waveletBank;
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
	static const int UPSAMP_FACTOR = 2;

	void forwardStep1d(const VectorXd& signal, VectorXd& approxCoefs, VectorXd& detailCoefs);
	void symmetricExtense(VectorXd& signal, size_t n);

	VectorXd inverseStep1d(const VectorXd& approxCoefs, const VectorXd& detailCoefs);

	Wavelet wavelet;
	int numLevels;
};

#endif // !WAVELET_TRANSFORM_H
