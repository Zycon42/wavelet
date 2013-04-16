/**
 * @file wavelettransform.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#define _USE_MATH_DEFINES
#include <cmath>

#include "wavelettransform.h"
#include "utils.h"

#include <algorithm>
#include <cassert>

std::map<WaveletFactory::Type, Wavelet> WaveletFactory::waveletBank = create_map<Type, Wavelet>
	(WaveletFactory::Type::Cdf97,
		Wavelet(create_vector<double>(-1.586134342)(-0.05298011854)(0.8829110762)(0.4435068522)(1.149604398))
	);

Wavelet WaveletFactory::create(Type type) {
	auto valit = waveletBank.find(type);
	if (valit == waveletBank.end())
		throw std::runtime_error("Bad wavelet type");

	return valit->second;
}

WaveletTransform::WaveletTransform(Wavelet wavelet, int numLevels) : 
	wavelet(std::move(wavelet)), numLevels(numLevels) {
}

WaveletTransform::~WaveletTransform() {
}

void WaveletTransform::liftPredict(VectorXd& signal, double coef) {
	for (size_t i = 1; i < signal.size() - 2; i += 2) {
		signal[i] += coef * (signal[i - 1] + signal[i + 1]);
	} 
	signal[signal.size() - 1] += 2 * coef * signal[signal.size() - 2];
}

void WaveletTransform::liftUpdate(VectorXd& signal, double coef) {
	for (size_t i = 2; i < signal.size(); i += 2) {
		signal[i] += coef * (signal[i - 1] + signal[i + 1]);
	}
	signal[0] += 2 * coef * signal[1];
}

void WaveletTransform::forwardStep1d(const VectorXd& signal, VectorXd& approxCoefs, VectorXd& detailCoefs) {
	VectorXd sig = signal;

	// predict 1
	liftPredict(sig, wavelet.coefs[0]);

	// update 1
	liftUpdate(sig, wavelet.coefs[1]);

	// predict 2
	liftPredict(sig, wavelet.coefs[2]);

	// update 2
	liftUpdate(sig, wavelet.coefs[3]);

	// scale and store result
	double scaleCoef = 1.0 / wavelet.coefs[4];
	for (size_t i = 0; i < sig.size(); ++i) {
		if (i % 2) {
			sig[i] *= scaleCoef;
			detailCoefs.push_back(sig[i]);
		} else {
			sig[i] /= scaleCoef;
			approxCoefs.push_back(sig[i]);
		}
	}
}

std::list<VectorXd> WaveletTransform::forward1d(const VectorXd& signal) {
	VectorXd processedSig = signal;
	
	std::list<VectorXd> result;
	VectorXd approxCoefs, detailCoefs;
	for (int i = 0; i < numLevels; ++i) {
		forwardStep1d(processedSig, approxCoefs, detailCoefs);
		// add detail coefs to result
		result.push_back(detailCoefs);

		// last iteration
		if (i == numLevels - 1) {
			// add to result also approx coefs
			result.push_back(approxCoefs);
		}

		// use approximated coefficients as signal in next iteration
		processedSig = approxCoefs;
		approxCoefs.clear();
		detailCoefs.clear();
	}

	return result;
}

VectorXd WaveletTransform::inverseStep1d(const VectorXd& approxCoefs, const VectorXd& detailCoefs) {
	assert(approxCoefs.size() == detailCoefs.size());

	VectorXd result(approxCoefs.size() + detailCoefs.size());
	double scaleCoef = wavelet.coefs[4];
	for (size_t i = 0; i < result.size() / 2; i++) {
		result[i * 2] = approxCoefs[i] / scaleCoef;
		result[i * 2 + 1] = detailCoefs[i] * scaleCoef;
	}

	// undo update 2
	liftUpdate(result, -wavelet.coefs[3]);
	
	// undo predict 2
	liftPredict(result, -wavelet.coefs[2]);
	
	// undo update 1
	liftUpdate(result, -wavelet.coefs[1]);
	
	// undo predict 1
	liftPredict(result, -wavelet.coefs[0]);

	return result;
}

VectorXd WaveletTransform::inverse1d(const std::list<VectorXd>& dwt) {
	assert(dwt.size() == numLevels + 1);

	// process dwt coefs from end
	auto curCoefIterator = dwt.rbegin();
	auto approxCoef = *curCoefIterator++;
	auto detailCoef = *curCoefIterator++;

	VectorXd result;
	for (int i = 0; i < numLevels; ++i) {
		result = inverseStep1d(approxCoef, detailCoef);

		if (i < numLevels - 1) {
			approxCoef = result;
			detailCoef = *curCoefIterator++;
		}
	}

	return result;
}