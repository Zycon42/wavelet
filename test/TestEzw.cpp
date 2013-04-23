#include <gtest/gtest.h>

#include <ezwdecoder.h>
#include <ezwencoder.h>

#include <sstream>

class TestEzw : public ::testing::Test
{
protected:
	void SetUp() {
		static const int example[8][8] = { 
			{ 63,-34, 49, 10,  7, 13,-12,  7 }, 
			{-31, 23, 14,-13,  3,  4,  6, -1 }, 
			{ 15, 14,  3,-12,  5, -7,  3,  9 }, 
			{ -9, -7,-14,  8,  4, -2,  3,  2 }, 
			{ -5,  9, -1, 47,  4,  6, -2,  2 }, 
			{  3,  0, -3,  2,  3, -2,  0,  4 }, 
			{  2, -3,  6, -4,  3,  6,  3,  6 }, 
			{  5, 11,  5,  6,  0,  3, -4,  4 } 
		};

		simpleData = cv::Mat(8, 8, CV_32S);
		for (int i = 0; i < simpleData.rows; ++i) {
			for (int j = 0; j < simpleData.cols; ++j) {
				simpleData.at<int32_t>(i, j) = example[i][j];
			}
		}
	}

	cv::Mat simpleData;
};

TEST_F(TestEzw, Simple) {
	std::ostringstream ostream;
	EzwEncoder ezwEncoder = EzwEncoder(BitStreamWriter(&ostream));
	int threshold;
	cv::Mat expected = simpleData.clone();
	ezwEncoder.encode(simpleData, threshold);

	std::istringstream istream(ostream.str());
	EzwDecoder ezwDecoder = EzwDecoder(BitStreamReader(&istream));
	cv::Mat decoded = cv::Mat::zeros(expected.rows, expected.cols, CV_32S);
	ezwDecoder.decode(threshold, decoded);

	for (int i = 0; i < expected.rows; ++i) {
		for (int j = 0; j < expected.cols; ++j) {
			EXPECT_EQ(expected.at<int32_t>(i, j), decoded.at<int32_t>(i, j));
		}
	}
}