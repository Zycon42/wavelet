#include <gtest/gtest.h>

#include <mathutils.h>

class TestMathUtils : public ::testing::Test
{
protected:
	virtual void SetUp() {
		static double sigVals[] = {5, 6, 7, 1, 2, 6, 2, 5};
		signal.assign(sigVals, sigVals + sizeof(sigVals) / sizeof(sigVals[0]));

		static double kernelVals[] = {1, 2, 1};
		kernel.assign(kernelVals, kernelVals + sizeof(kernelVals) / sizeof(kernelVals[0]));
	}

	VectorXd signal;
	VectorXd kernel;
};

TEST_F(TestMathUtils, ConvolutionFull) {
	VectorXd y = convolve(signal, kernel);

	EXPECT_EQ(10, y.size());
	static double expected[] = {5, 16, 24, 21, 11, 11, 16, 15, 12, 5};
	for (size_t i = 0; i < y.size(); ++i) {
		EXPECT_DOUBLE_EQ(expected[i], y[i]);
	}

	// Kernel larger than signal
	y = convolve(kernel, signal);
	// should return same as above
	EXPECT_EQ(10, y.size());
	for (size_t i = 0; i < y.size(); ++i) {
		EXPECT_DOUBLE_EQ(expected[i], y[i]);
	}
}

TEST_F(TestMathUtils, ConvolutionSame) {
	VectorXd y = convolve(signal, kernel, ConvolveMode::Same);

	EXPECT_EQ(8, y.size());
	static double expected[] = {16, 24, 21, 11, 11, 16, 15, 12};
	for (size_t i = 0; i < y.size(); ++i) {
		EXPECT_DOUBLE_EQ(expected[i], y[i]);
	}
}

TEST_F(TestMathUtils, ConvolutionValid) {
	VectorXd y = convolve(signal, kernel, ConvolveMode::Valid);

	EXPECT_EQ(6, y.size());
	static double expected[] = {24, 21, 11, 11, 16, 15};
	for (size_t i = 0; i < y.size(); ++i) {
		EXPECT_DOUBLE_EQ(expected[i], y[i]);
	}
}