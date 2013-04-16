#include <gtest/gtest.h>

#include <wavelettransform.h>

#include <memory>

class TestDwt : public ::testing::Test
{
protected:
	void SetUp() {
		for (int i = 0; i < 32; i++) {
			testVal.push_back(5.0 + i + 0.4 * i * i - 0.02 * i * i * i);
		}

		cdf97Wt.reset(new WaveletTransform(WaveletFactory::create(WaveletFactory::Type::Cdf97), 1));
	}

	std::unique_ptr<WaveletTransform> harrWt;
	std::unique_ptr<WaveletTransform> cdf97Wt;

	VectorXd testVal;
};

TEST_F(TestDwt, OneDimCdf97) {
	auto forwardRes = cdf97Wt->forward1d(testVal);
	auto inverseRes = cdf97Wt->inverse1d(forwardRes);

	ASSERT_EQ(testVal.size(), inverseRes.size());
	for (size_t i = 0; i < testVal.size(); ++i) {
		EXPECT_DOUBLE_EQ(testVal[i], inverseRes[i]);
	}
}