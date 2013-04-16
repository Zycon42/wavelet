/**
 * @file wavelettransform.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef WAVELET_TRANSFORM_H
#define WAVELET_TRANSFORM_H

#include <vector>
#include <list>
#include <map>

typedef std::vector<double> VectorXd;

struct Wavelet
{
	Wavelet() {}
	Wavelet(VectorXd coefs) : coefs(std::move(coefs)) {}

	VectorXd coefs;
};

class WaveletFactory
{
public:
	enum class Type { Cdf97 };
	static Wavelet create(Type type);
private:
	static std::map<Type, Wavelet> waveletBank;
};

class WaveletTransform
{
public:
	WaveletTransform(Wavelet wavelet, int numLevels);
	~WaveletTransform();

	std::list<VectorXd> forward1d(const VectorXd& signal);

	VectorXd inverse1d(const std::list<VectorXd>& dwt);
private:

	static void liftPredict(VectorXd& signal, double coef);
	static void liftUpdate(VectorXd& signal, double coef);
	void forwardStep1d(const VectorXd& signal, VectorXd& approxCoefs, VectorXd& detailCoefs);
	VectorXd inverseStep1d(const VectorXd& approxCoefs, const VectorXd& detailCoefs);

	Wavelet wavelet;
	int numLevels;
};

#endif // !WAVELET_TRANSFORM_H
