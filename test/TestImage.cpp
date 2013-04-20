#include <gtest/gtest.h>

#include <wlfimage.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

double computeDifference(const cv::Mat& test, const cv::Mat& ref) {
	cv::Mat diff;
	cv::absdiff(test, ref, diff);
	return cv::sum(diff).val[0] / (diff.rows*diff.cols);
}

TEST(TestImage, Simple) {
	cv::Mat image = cv::imread("lena.png", CV_LOAD_IMAGE_COLOR);
	ASSERT_FALSE(!image.data);

	WlfImage::save("lena.wlf", image);

	cv::Mat read = WlfImage::read("lena.wlf");
	cv::imwrite("lena-inv.png", read);

	EXPECT_NEAR(0.0, computeDifference(read, image), 1.0);
}