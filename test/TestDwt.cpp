#include <gtest/gtest.h>

#include <wavelettransform.h>
#include <cdf97wavelet.h>
#include <cdf53wavelet.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <memory>
#include <cstdlib>
#include <algorithm>

class TestDwt : public ::testing::Test
{
protected:
	void SetUp() {
	}

	double computeDifference(const cv::Mat& test, const cv::Mat& ref) {
		cv::Mat diff;
		cv::absdiff(test, ref, diff);
		return cv::sum(diff).val[0] / (diff.rows*diff.cols);
	}

};

TEST_F(TestDwt, TwoDimCdf97) {
	std::unique_ptr<WaveletTransform> cdf97Wt(WaveletTransformFactory::create<Cdf97Wavelet>(2));

	cv::Mat image = cv::imread("lena.png", CV_LOAD_IMAGE_COLOR);
	ASSERT_FALSE(!image.data);

	cv::Mat dimg;
	image.convertTo(dimg, CV_32FC3);
	dimg = dimg.reshape(1);

	cv::Mat orgDimg = dimg.clone();

	cdf97Wt->forward2d(dimg);
	//cv::imwrite("dwt.png", dimg.reshape(3));

	cdf97Wt->inverse2d(dimg);
	//cv::imwrite("inverse.png", dimg.reshape(3));

	EXPECT_NEAR(0.0, computeDifference(dimg, orgDimg), 1e-4);
}

TEST_F(TestDwt, TwoDimCdf53) {
	std::unique_ptr<WaveletTransform> cdf53Wt(WaveletTransformFactory::create<Cdf53Wavelet>(1));

	cv::Mat image = cv::imread("lena.png", CV_LOAD_IMAGE_COLOR);
	ASSERT_FALSE(!image.data);

	cv::Mat dimg;
	image.convertTo(dimg, CV_32SC3);
	dimg = dimg.reshape(1);

	cv::Mat orgDimg = dimg.clone();

	cdf53Wt->forward2d(dimg);
	cv::imwrite("dwt.png", dimg.reshape(3));

	cdf53Wt->inverse2d(dimg);
	cv::imwrite("inverse.png", dimg.reshape(3));

	EXPECT_DOUBLE_EQ(0.0, computeDifference(dimg, orgDimg));
}

TEST_F(TestDwt, OneDimCdf53) {
	auto cdf53wavelet = Cdf53Wavelet();
	std::vector<int> data(512);
	std::generate(data.begin(), data.end(), [] () { return rand() % 255; });

	auto origData = data;

	cdf53wavelet.forward(data);
	cdf53wavelet.inverse(data);

	for (size_t i = 0; i < origData.size(); i++) {
		EXPECT_EQ(origData[i], data[i]);
	}
}