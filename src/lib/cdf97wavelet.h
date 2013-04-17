/**
 * @file cdf97wavelet.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef CDF97_WAVELET_H
#define CDF97_WAVELET_H

#include "wavelet.h"

#include <vector>

/**
 * Biorthogonal Cohen-Daubechies-Feauveau 9/7 wavelet.
 * Lifting implementation
 */
class Cdf97Wavelet : public Wavelet
{
public:
	virtual void forward(ArrayRef<double> signal);

	virtual void inverse(ArrayRef<double> dwt);
private:
	static void liftPredict(ArrayRef<double> signal, double coef);
	static void liftUpdate(ArrayRef<double> signal, double coef);

	static double coefs[];				// lifting coeficients
	std::vector<double> tempbank;		// temp buffer for de/interleaving signal
};

#endif // !CDF97_WAVELET_H
