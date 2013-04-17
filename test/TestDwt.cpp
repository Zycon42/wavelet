#include <gtest/gtest.h>

#include <wavelettransform.h>
#include <cdf97wavelet.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <memory>

class TestDwt : public ::testing::Test
{
protected:
	void SetUp() {
		for (int i = 0; i < 32; i++) {
			testVal.push_back(5.0 + i + 0.4 * i * i - 0.02 * i * i * i);
		}

		cdf97Wt.reset(new WaveletTransform(std::make_shared<Cdf97Wavelet>(), 2));
	}

	double computeDifference(const cv::Mat& test, const cv::Mat& ref) {
		cv::Mat diff;
		cv::absdiff(test, ref, diff);
		return cv::sum(diff).val[0] / (diff.rows*diff.cols);
	}

	std::unique_ptr<WaveletTransform> cdf97Wt;

	VectorXd testVal;
};

TEST_F(TestDwt, OneDimCdf97) {
	auto working = testVal;
	cdf97Wt->forward1d(working);
	cdf97Wt->inverse1d(working);

	ASSERT_EQ(testVal.size(), working.size());
	for (size_t i = 0; i < testVal.size(); ++i) {
		EXPECT_NEAR(testVal[i], working[i], 1e-13);
	}
}

TEST_F(TestDwt, TwoDimCdf97) {
	cv::Mat image = cv::imread("lena.png", CV_LOAD_IMAGE_GRAYSCALE);
	ASSERT_FALSE(!image.data);

	cv::Mat dimg;
	image.convertTo(dimg, CV_64F);
	cv::Mat orgDimg = dimg.clone();

	cdf97Wt->forward2d(dimg);
	//cv::imwrite("dwt.png", dimg);

	cdf97Wt->inverse2d(dimg);
	//cv::imwrite("inverse.png", dimg);

	EXPECT_NEAR(0.0, computeDifference(dimg, orgDimg), 1e-13);
}