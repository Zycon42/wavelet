/**
 * @file wavelettransform.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "wavelettransform.h"

#include <cassert>

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
	signal.insert(signal.begin(), n);
	signal.insert(signal.end(), n);

	// insert values symmetrically
	for (size_t i = 0; i < n; ++i) {
		signal[i] = signal[2 * n - 1 - i];
		signal[signal.size() - 1 -i] = signal[signal.size() - 2 * n - i];
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
		// TODO: symmetric signal extension
		size_t extSize = wavelet.lowPassAnalysisFilter.size();
		symmetricExtense(processedSig, extSize);

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

VectorXd WaveletTransform::inverse1d(const std::list<VectorXd>& dwt) {

}