/**
 * @file wavelettransform.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef WAVELET_TRANSFORM_H
#define WAVELET_TRANSFORM_H

#include "wavelet.h"

#include <opencv2/core/core.hpp>

#include <utility>
#include <memory>
#include <list>

class WaveletTransform
{
public:
	virtual ~WaveletTransform() { }

	virtual int getType() = 0;

	virtual void forward2d(cv::Mat& signal) = 0;

	virtual void inverse2d(cv::Mat& dwt) = 0;
};

template <typename T>
struct WaveletTransformTraits
{
	static const int cvMatType = -1;
};

template <>
struct WaveletTransformTraits<float>
{
	static const int cvMatType = CV_32F;
};

template <>
struct WaveletTransformTraits<int32_t>
{
	static const int cvMatType = CV_32S;
};

template <typename T, class Traits = WaveletTransformTraits<T>>
class WaveletTransformImpl : public WaveletTransform
{
public:
	typedef Traits traits_type;
	typedef T value_type;
	typedef Wavelet<value_type> wavelet_type;

	WaveletTransformImpl(const std::shared_ptr<wavelet_type>& wavelet, int numLevels);

	void forward1d(std::vector<value_type>& signal);

	void inverse1d(std::vector<value_type>& dwt);

	virtual int getType() {
		return traits_type::cvMatType;
	}

	virtual void forward2d(cv::Mat& signal);

	virtual void inverse2d(cv::Mat& dwt);
private:
	std::shared_ptr<wavelet_type> wavelet;
	int numLevels;
};

class WaveletTransformFactory
{
public:
	template <typename WaveletType>
	static WaveletTransform* create(const std::shared_ptr<
		typename std::enable_if<std::is_base_of<WaveletBase, WaveletType>::value, WaveletType>::type>& wavelet, 
		int numlevels) {
		
		return new WaveletTransformImpl<typename WaveletType::impl_type>(wavelet, numlevels);
	}

	template <typename WaveletType>
	static typename std::enable_if<std::is_base_of<WaveletBase, WaveletType>::value, WaveletTransform>::type* create(int numlevels) {
		return new WaveletTransformImpl<typename WaveletType::impl_type>(std::make_shared<WaveletType>(), numlevels);
	}
};

#endif // !WAVELET_TRANSFORM_H
