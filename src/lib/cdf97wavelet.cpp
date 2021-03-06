/**
 * @file cdf97wavelet.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "cdf97wavelet.h"

#include <cassert>

float Cdf97Wavelet::coefs[] = {
	-1.586134342060f, -0.052980118573f, 0.882911075531f, 0.443506852044f, 1.149604398860f
};

void Cdf97Wavelet::forward(ArrayRef<float> signal) {
	assert(signal.size() % 2 == 0);

	// predict 1
	liftPredict(signal, coefs[0]);

	// update 1
	liftUpdate(signal, coefs[1]);

	// predict 2
	liftPredict(signal, coefs[2]);

	// update 2
	liftUpdate(signal, coefs[3]);

	tempbank.resize(signal.size());

	// scale and store result
	float scaleCoef = 1.0f / coefs[4];
	for (size_t i = 0; i < signal.size(); ++i) {
		if (i % 2) {
			signal[i] *= scaleCoef;
			tempbank[i / 2 + signal.size() / 2] = signal[i];
		} else {
			signal[i] /= scaleCoef;
			tempbank[i / 2] = signal[i];
		}
	}

	for (size_t i = 0; i < signal.size(); i++) {
		signal[i] = tempbank[i];
	}
}

void Cdf97Wavelet::inverse(ArrayRef<float> dwt) {
	assert(dwt.size() % 2 == 0);

	tempbank.resize(dwt.size());
	// unscale and interleave coefs
	float scaleCoef = coefs[4];
	for (size_t i = 0; i < dwt.size() / 2; i++) {
		tempbank[i * 2] = dwt[i] / scaleCoef;
		tempbank[i * 2 + 1] = dwt[i + dwt.size() / 2] * scaleCoef;
	}

	for (size_t i = 0; i < dwt.size(); i++) {
		dwt[i] = tempbank[i];
	}

	// undo update 2
	liftUpdate(dwt, -coefs[3]);

	// undo predict 2
	liftPredict(dwt, -coefs[2]);

	// undo update 1
	liftUpdate(dwt, -coefs[1]);

	// undo predict 1
	liftPredict(dwt, -coefs[0]);
}

void Cdf97Wavelet::liftPredict(ArrayRef<float> signal, float coef) {
	for (size_t i = 1; i < signal.size() - 2; i += 2) {
		signal[i] += coef * (signal[i - 1] + signal[i + 1]);
	}
	// symmetric extension
	signal[signal.size() - 1] += 2 * coef * signal[signal.size() - 2];
}

void Cdf97Wavelet::liftUpdate(ArrayRef<float> signal, float coef) {
	for (size_t i = 2; i < signal.size(); i += 2) {
		signal[i] += coef * (signal[i - 1] + signal[i + 1]);
	}
	// symmetric extension
	signal[0] += 2 * coef * signal[1];
}