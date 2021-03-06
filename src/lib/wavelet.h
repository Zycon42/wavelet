/**
 * @file wavelet.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef WAVELET_H
#define WAVELET_H

#include "utils.h"

/// this class is needed, because we want some nontemplate Wavelet base class
/// for restriction in WaveletFactory
class WaveletBase { protected: WaveletBase() { } };

/**
 * Base class for all wavelets.
 * @tparam T type on which wavelet operates (e.g. float,int etc.)
 */
template <typename T>
class Wavelet : public WaveletBase
{
public:
	typedef T impl_type;

	virtual ~Wavelet() { }

	/**
	 * Performs one level in place dwt with this wavelet.
	 * @param signal input signal that will be replaced with its dwt. 
	 *     Its size must be even. First half will contain approx coefs and
	 *     second half detail coefs.
	 */
	virtual void forward(ArrayRef<T> signal) = 0;

	/**
	 * Performs one level in place idwt with this wavelet.
	 * @param dwt result from forward method that will be replaced with
	 *     its original if their size is same
	 */
	virtual void inverse(ArrayRef<T> dwt) = 0;
};

#endif // !WAVELET_H
