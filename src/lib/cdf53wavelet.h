/**
 * @file cdf53wavelet.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef CDF53_WAVELET_H
#define CDF53_WAVELET_H

#include "wavelet.h"

#include <cstdint>

/**
 * Biorthogonal Cohen-Daubechies-Feauveau 5/3 wavelet AKA Le-Gall 5/3.
 * Lifting integer implementation
 */
class Cdf53Wavelet : public Wavelet<int32_t>
{
public:
	virtual void forward(ArrayRef<int32_t> signal);

	virtual void inverse(ArrayRef<int32_t> dwt);
private:
	std::vector<int32_t> tempbank;		// temp buffer for de/interleaving signal
};

#endif // !CDF53_WAVELET_H
