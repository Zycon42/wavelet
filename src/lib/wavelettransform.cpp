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

template <typename T, class Traits>
WaveletTransformImpl<T, Traits>::WaveletTransformImpl(const std::shared_ptr<wavelet_type>& wavelet, int numLevels) : 
	wavelet(wavelet), numLevels(numLevels) {
}

template <typename T, class Traits>
void WaveletTransformImpl<T, Traits>::forward1d(std::vector<value_type>& signal) {
	// signal must be 2^numlevels long
	if (signal.size() % (1 << numLevels) != 0)
		throw std::runtime_error("WaveletTransform::forward1d: signal must be 2^numLevels long!");

	size_t size = signal.size();
	for (int i = 0; i < numLevels; ++i) {
		wavelet->forward(ArrayRef<value_type>(signal.data(), size));
		size /= 2;
	}
}

template <typename T, class Traits>
void WaveletTransformImpl<T, Traits>::inverse1d(std::vector<value_type>& dwt) {
	// input must be 2^numlevels long
	if (dwt.size() % (1 << numLevels) != 0)
		throw std::runtime_error("WaveletTransform::inverse1d: input must be 2^numLevels long!");

	// get second highest level size
	size_t size = dwt.size() / (1 << (numLevels - 1));
	for (int i = 0; i < numLevels; ++i) {
		wavelet->inverse(ArrayRef<value_type>(dwt.data(), size));
		size *= 2;
	}
}

template <typename T, class Traits>
void WaveletTransformImpl<T, Traits>::forward2d(cv::Mat& signal) {
	assert(signal.type() == traits_type::cvMatType);

	cv::Mat roi(signal, cv::Rect(0, 0, signal.cols, signal.rows));
	for (int i = 0; i < numLevels; ++i) {
		// transform rows
		for (int i = 0; i < roi.rows; ++i) {
			ArrayRef<value_type> rowPtr(roi.ptr<value_type>(i), roi.cols);
			wavelet->forward(rowPtr);
		}
		// transpose matrix so col transformation can be done in rows
		cv::Mat transposed = roi.t();
		for (int i = 0; i < transposed.rows; ++i) {
			ArrayRef<value_type> rowPtr(transposed.ptr<value_type>(i), transposed.cols);
			wavelet->forward(rowPtr);
		}
		// copy transposed to roi
		cv::Mat(transposed.t()).copyTo(roi);

		// set roi to upper left corner
		roi.adjustROI(0, -(roi.rows / 2), 0, -(roi.cols / 2));
	}
}

template <typename T, class Traits>
void WaveletTransformImpl<T, Traits>::inverse2d(cv::Mat& dwt) {
	assert(dwt.type() == traits_type::cvMatType);

	size_t factor = 1 << (numLevels - 1);
	cv::Mat roi(dwt, cv::Rect(cv::Point(0, 0), cv::Size(dwt.cols / factor, dwt.rows / factor)));
	for (int i = 0; i < numLevels; ++i) {
		// transform rows
		for (int i = 0; i < roi.rows; ++i) {
			ArrayRef<value_type> rowPtr(roi.ptr<value_type>(i), roi.cols);
			wavelet->inverse(rowPtr);
		}
		// transpose matrix so col transformation can be done in rows
		cv::Mat transposed = roi.t();
		for (int i = 0; i < transposed.rows; ++i) {
			ArrayRef<value_type> rowPtr(transposed.ptr<value_type>(i), transposed.cols);
			wavelet->inverse(rowPtr);
		}
		// copy transposed to roi
		cv::Mat(transposed.t()).copyTo(roi);

		// extend roi
		roi.adjustROI(0, roi.rows, 0, roi.cols);
	}
}

template class WaveletTransformImpl<int32_t>;
template class WaveletTransformImpl<float>;