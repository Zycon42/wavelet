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

VectorXd convolve(const VectorXd& signal, const VectorXd& kernel, ConvolveMode mode = ConvolveMode::Full);

VectorXd downsample(const VectorXd& signal, size_t factor);

#endif // !MATH_UTILS_H
