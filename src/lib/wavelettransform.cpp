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
	(WaveletFactory::Type::Harr, 
		Wavelet(create_vector<double>(M_SQRT1_2)(M_SQRT1_2), create_vector<double>(-M_SQRT1_2)(M_SQRT1_2), 
			create_vector<double>(M_SQRT1_2)(M_SQRT1_2), create_vector<double>(M_SQRT1_2)(-M_SQRT1_2)
		)
	)(WaveletFactory::Type::Cdf97,
		Wavelet(create_vector<double>(0.026748757411)(-0.016864118443)(-0.078223266529)(0.266864118443)
				(0.602949018236)(0.266864118443)(-0.078223266529)(-0.016864118443)(0.026748757411),
			create_vector<double>(0.0)(0.091271763114)(-0.057543526229)(-0.591271763114)(1.11508705)
				(-0.591271763114)(-0.057543526229)(0.091271763114)(0.0),
			create_vector<double>(0.0)(-0.091271763114)(-0.057543526229)(0.591271763114)(1.11508705)
				(0.591271763114)(-0.057543526229)(-0.091271763114)(0.0),
			create_vector<double>(0.026748757411)(0.016864118443)(-0.078223266529)(-0.266864118443)
				(0.602949018236)(-0.266864118443)(-0.078223266529)(0.016864118443)(0.026748757411)
		)
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

void WaveletTransform::forwardStep1d(const VectorXd& signal, VectorXd& approxCoefs, VectorXd& detailCoefs) {
	auto lpTemp = convolve(signal, wavelet.lowPassAnalysisFilter, ConvolveMode::Valid);
	downsample(lpTemp, DOWNSAMP_FACTOR, approxCoefs);

	auto hpTemp = convolve(signal, wavelet.highPassAnalysisFilter, ConvolveMode::Valid);
	downsample(hpTemp, DOWNSAMP_FACTOR, detailCoefs);
}

void WaveletTransform::symmetricExtense(VectorXd& signal, size_t n) {
	// we must have enough values for extension
	assert(signal.size() > 2 * n);

	// preallocate room in signal
	signal.insert(signal.begin(), n, VectorXd::value_type());
	signal.insert(signal.end(), n, VectorXd::value_type());

	// insert values symmetrically
	for (size_t i = 0; i < n; ++i) {
		signal[i] = signal[2 * n - 1 - i];
		signal[signal.size() - 1 - i] = signal[signal.size() - 2 * n + i];
	}
}

std::list<VectorXd> WaveletTransform::forward1d(const VectorXd& signal) {
	VectorXd processedSig = signal;
	// make signal even
	if ((signal.size() % 2) != 0) {
		processedSig.push_back(signal.back());
	}

	std::list<VectorXd> result;
	VectorXd approxCoefs, detailCoefs;
	for (int i = 0; i < numLevels; ++i) {
		// symmetric signal extension
		size_t extSize = wavelet.lowPassAnalysisFilter.size();
		symmetricExtense(processedSig, extSize - 1);

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
	VectorXd appUpsampled = upsample(approxCoefs, UPSAMP_FACTOR);
	appUpsampled.pop_back();
	VectorXd appConv = convolve(appUpsampled, wavelet.lowPassSynthesisFilter, ConvolveMode::Valid);

	VectorXd detUpsampled = upsample(detailCoefs, UPSAMP_FACTOR);
	detUpsampled.pop_back();
	VectorXd detConv = convolve(detUpsampled, wavelet.highPassSynthesisFilter, ConvolveMode::Valid);

	VectorXd result(appConv.size());
	// add appConv with dettConv to result
	std::transform(appConv.begin(), appConv.end(), detConv.begin(), result.begin(), 
		[] (double lhs, double rhs) -> double { return lhs + rhs; });

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