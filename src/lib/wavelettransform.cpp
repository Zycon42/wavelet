/**
 * @file wavelettransform.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#define _USE_MATH_DEFINES
#include <cmath>

#include "wavelettransform.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>

WaveletTransform::WaveletTransform(std::shared_ptr<Wavelet> wavelet, int numLevels) : 
	wavelet(std::move(wavelet)), numLevels(numLevels) {
}

WaveletTransform::~WaveletTransform() {
}

void WaveletTransform::forward1d(VectorXd& signal) {
	// signal must be 2^numlevels long
	if (signal.size() % (1 << numLevels) != 0)
		throw std::runtime_error("WaveletTransform::forward1d: signal must be 2^numLevels long!");

	size_t size = signal.size();
	for (int i = 0; i < numLevels; ++i) {
		wavelet->forward(ArrayRef<double>(signal.data(), size));
		size /= 2;
	}
}

void WaveletTransform::inverse1d(VectorXd& dwt) {
	// input must be 2^numlevels long
	if (dwt.size() % (1 << numLevels) != 0)
		throw std::runtime_error("WaveletTransform::inverse1d: input must be 2^numLevels long!");

	// get second highest level size
	size_t size = dwt.size() / (1 << (numLevels - 1));
	for (int i = 0; i < numLevels; ++i) {
		wavelet->inverse(ArrayRef<double>(dwt.data(), size));
		size *= 2;
	}
}
