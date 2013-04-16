#include <gtest/gtest.h>

#include <wavelettransform.h>

#include <memory>

class TestDwt : public ::testing::Test
{
protected:
	void SetUp() {
		for (int i = 0; i < 50; i++) {
			testVal.push_back(rand() % 50);
		}

		harrWt.reset(new WaveletTransform(WaveletFactory::create(WaveletFactory::Type::Harr), 2));
		cdf97Wt.reset(new WaveletTransform(WaveletFactory::create(WaveletFactory::Type::Cdf97), 2));
	}

	std::unique_ptr<WaveletTransform> harrWt;
	std::unique_ptr<WaveletTransform> cdf97Wt;

	VectorXd testVal;
};

/*TEST_F(TestDwt, OneDimHarr) {
	auto forwardRes = harrWt->forward1d(testVal);
	auto inverseRes = harrWt->inverse1d(forwardRes);

	ASSERT_EQ(testVal.size(), inverseRes.size());
	for (size_t i = 0; i < testVal.size(); ++i) {
		EXPECT_NEAR(testVal[i], inverseRes[i], 1e-13);
	}
}*/

TEST_F(TestDwt, OneDimCdf97) {
	auto forwardRes = cdf97Wt->forward1d(testVal);
	auto inverseRes = cdf97Wt->inverse1d(forwardRes);

	ASSERT_EQ(testVal.size(), inverseRes.size());
	for (size_t i = 0; i < testVal.size(); ++i) {
		EXPECT_NEAR(testVal[i], inverseRes[i], 1e-13);
	}
}