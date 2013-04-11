#include <gtest/gtest.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

TEST(TestCV, ImgRead) {
	cv::Mat image = cv::imread("lena.png");
	EXPECT_FALSE(!image.data);
}