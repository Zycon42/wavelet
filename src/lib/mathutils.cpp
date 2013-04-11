/**
 * @file mathutils.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "mathutils.h"

#include <cmath>

VectorXd convolve(const VectorXd& signal, const VectorXd& kernel, ConvolveMode mode /* = ConvolveMode::Full */) {
	// perform full discrete linear convolution
	VectorXd result(signal.size() + kernel.size() - 1);
	for (size_t n = 0; n < result.size(); ++n) {
		double sum = 0.0;
		for (size_t m = 0; m < kernel.size(); ++m) {
			if (m <= n && (n - m) < signal.size())
				sum += signal[n - m] * kernel[m];
		}
		result[n] = sum;
	}

	// return sliced result according to given mode
	if (mode == ConvolveMode::Valid) {
		size_t pad = std::min(signal.size(), kernel.size()) - 1;
		return VectorXd(result.begin() + pad, result.end() - pad);
	} else if (mode == ConvolveMode::Same) {
		size_t resultSize = std::max(signal.size(), kernel.size());
		size_t sizeDiff = result.size() - resultSize;
		auto resBegin = result.begin() + sizeDiff / 2;
		return VectorXd(resBegin, resBegin + resultSize);
	} else
		return result;
}

VectorXd downsample(const VectorXd& signal, size_t factor) {
	size_t resultLen = std::ceil((double)signal.size() / factor);
	VectorXd result(resultLen);
	for (size_t i = 0; i < resultLen; ++i) {
		result[i] = signal[i * factor];
	}

	return result;
}

size_t downsample(const VectorXd& signal, size_t factor, VectorXd& result) {
	size_t resultLen = std::ceil((double)signal.size() / factor);
	for (size_t i = 0; i < resultLen; ++i) {
		result.push_back(signal[i * factor]);
	}

	return resultLen;
}

VectorXd upsample(const VectorXd& signal, size_t factor) {
	size_t resultLen = std::ceil((double)signal.size() * factor);
	VectorXd result(resultLen);
	for (size_t i = 0; i < resultLen; ++i) {
		if (i % factor == 0)
			result[i] = signal[i / factor];
		else
			result[i] = 0.0;
	}
	return result;
}

size_t upsample(const VectorXd& signal, size_t factor, VectorXd& result) {
	size_t resultLen = std::ceil((double)signal.size() * factor);
	for (size_t i = 0; i < resultLen; ++i) {
		if (i % factor == 0)
			result.push_back(signal[i / factor]);
		else
			result.push_back(0.0);
	}

	return resultLen;
}