/**
 * @file mathutils.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <vector>

typedef std::vector<double> VectorXd;

enum class ConvolveMode {
	Full, Same, Valid
};

/**
 * Perform discrete linear convolution of signal with kernel. 
 * Note that this operation is commutative.
 * @param signal left hand side of convolution operator
 * @param kernel right hand side of convolution operator
 * @param mode parameter that determines size of returned result, defaults to Full.
 *        Full: full conv with size lhs+rhs-1 Valid: result consists only from
 *        elements that don't rely on padding Same: result is subset of full that
 *        its length is max(lhs,rhs)
 * @return result of convolution with length specified by mode parameter
 */
VectorXd convolve(const VectorXd& signal, const VectorXd& kernel, ConvolveMode mode = ConvolveMode::Full);

/**
 * Downsamples signal with factor.
 * @param signal input signal which will be downsampled
 * @param factor
 * @return downsampled signal
 */
VectorXd downsample(const VectorXd& signal, size_t factor);

/**
 * Downsamples signal with factor.
 * @param signal input signal which will be downsampled
 * @param factor
 * @retval result downsampled signal will be appended to this array
 * @return downsampled signal size
 */
size_t downsample(const VectorXd& signal, size_t factor, VectorXd& result);

/**
 * Upsamples signal with factor.
 * Added samples will be zeros.
 * @param signal input signal which will be upsampled
 * @param factor
 * @return upsampled signal
 */
VectorXd upsample(const VectorXd& signal, size_t factor);

/**
 * Upsamples signal with factor.
 * Added samples will be zeros.
 * @param signal input signal which will be upsampled
 * @param factor
 * @retval result upsampled signal will be appended to this array
 * @return upsampled signal size
 */
size_t upsample(const VectorXd& signal, size_t factor, VectorXd& result);

#endif // !MATH_UTILS_H
