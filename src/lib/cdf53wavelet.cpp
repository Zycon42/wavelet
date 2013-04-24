/**
 * @file cdf53wavelet.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "cdf53wavelet.h"

#include <cassert>

void Cdf53Wavelet::forward(ArrayRef<int32_t> signal) {
	assert(signal.size() % 2 == 0);

	// predict
	for (size_t i = 1; i < signal.size() - 2; i += 2) {
		signal[i] -= (signal[i - 1] + signal[i + 1]) >> 1;
	}
	// symmetric extension
	signal[signal.size() - 1] -= (2 * signal[signal.size() - 2]) >> 1;

	// update
	for (size_t i = 2; i < signal.size(); i += 2) {
		signal[i] += (signal[i - 1] + signal[i + 1] + 2) >> 2;
	}
	// symmetric extension
	signal[0] += (2 * signal[1] + 2) >> 2;

	// deinterleave to tempbank
	tempbank.resize(signal.size());
	for (size_t i = 0; i < signal.size(); ++i) {
		if (i % 2) {
			tempbank[i / 2 + signal.size() / 2] = signal[i];
		} else {
			tempbank[i / 2] = signal[i];
		}
	}

	for (size_t i = 0; i < signal.size(); i++) {
		signal[i] = tempbank[i];
	}
}

void Cdf53Wavelet::inverse(ArrayRef<int32_t> dwt) {
	assert(dwt.size() % 2 == 0);

	// interleave coefs
	tempbank.resize(dwt.size());
	for (size_t i = 0; i < dwt.size() / 2; i++) {
		tempbank[i * 2] = dwt[i];
		tempbank[i * 2 + 1] = dwt[i + dwt.size() / 2];
	}

	for (size_t i = 0; i < dwt.size(); i++) {
		dwt[i] = tempbank[i];
	}

	// undo update
	for (size_t i = 2; i < dwt.size(); i += 2) {
		dwt[i] -= (dwt[i - 1] + dwt[i + 1] + 2) >> 2;
	}
	// symmetric extension
	dwt[0] -= (2 * dwt[1] + 2) >> 2;

	// undo predict
	for (size_t i = 1; i < dwt.size() - 2; i += 2) {
		dwt[i] += (dwt[i - 1] + dwt[i + 1]) >> 1;
	}
	// symmetric extension
	dwt[dwt.size() - 1] += (2 * dwt[dwt.size() - 2]) >> 1;
}