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

/**
 * Wavelet transform interface
 */
class WaveletTransform
{
public:
	virtual ~WaveletTransform() { }

	/// Get OpenCV type on which transform operates.
	virtual int getType() = 0;

	/// Computes forward 2d dwt of signal
	virtual void forward2d(cv::Mat& signal) = 0;

	/// Computes inverse 2d dwt
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

/**
 * Wavelet transform implementation.
 * @tparam T type on which transform operates (currently only float and int32_t allowed)
 * @tparam Traits traits type for some specific type related values
 */
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

// Extern template instantiation, actual instantiation is done in 
// walettransform.cpp, by using this we save some compile time.
// Instantiation of WaveletTransformImpl with other types than those
// listed here will fail on horrible linked errors!
extern template class WaveletTransformImpl<int32_t>;
extern template class WaveletTransformImpl<float>;

/**
 * Factory for wavelet transform that creates right WaveletTransform object for given wavelet.
 */
class WaveletTransformFactory
{
public:
	/**
	 * Creates WaveletTransform with given wavelet object, and number of levels.
	 * @param wavelet shared_ptr to some wavelet
	 * @param numlevels number of dwt levels
	 *
	 * @tparam WaveletType some type that derives from Wavelet<T>
	 */
	template <typename WaveletType>
	static typename std::enable_if<std::is_base_of<WaveletBase, WaveletType>::value, WaveletTransform>::type* create(
		const std::shared_ptr<WaveletType>& wavelet, 
		int numlevels) {
		
		return new WaveletTransformImpl<typename WaveletType::impl_type>(wavelet, numlevels);
	}

	/**
	 * Creates WaveletTransform with given wavelet type, and number of levels.
	 * Wavelet object of type WaveletTYpe is constructed so if you want to 
	 * supply your own wavelet object use other overload
	 * @param numlevels number of dwt levels
	 *
	 * @tparam WaveletType some type that derives from Wavelet<T>
	 */
	template <typename WaveletType>
	static typename std::enable_if<std::is_base_of<WaveletBase, WaveletType>::value, WaveletTransform>::type* create(int numlevels) {
		return new WaveletTransformImpl<typename WaveletType::impl_type>(std::make_shared<WaveletType>(), numlevels);
	}
};

#endif // !WAVELET_TRANSFORM_H
