#include <gtest/gtest.h>

#include "arithmdecoder.h"
#include "arithmencoder.h"

#include <sstream>
#include <vector>

class TestAC : public ::testing::Test
{
protected:
	void SetUp() {
		static int data[] = { 0, 2, 3, 1, 3, 1, 0, 2, 0, 1, 1, 3, 0, 2, 1, 3, 1, 3, 3, 2, 0 };
		simpleData.assign(data, data + sizeof(data) / sizeof(*data));
	}

	std::vector<int> simpleData;
};

TEST_F(TestAC, DynamicDataModel) {
	std::ostringstream os;

	AdaptiveDataModel dataModel(4);

	auto ae = ArithmeticEncoder(std::make_shared<BitStreamWriter>(&os));
	for (auto val : simpleData) {
		ae.encode(val, &dataModel);
	}
	ae.reset();
	dataModel.reset();

	auto encoded = os.str();

	std::istringstream is(encoded);
	auto ad = ArithmeticDecoder(std::make_shared<BitStreamReader>(&is));
	for (size_t i = 0; i < simpleData.size(); ++i) {
		auto decoded = ad.decode(&dataModel);
		EXPECT_EQ(simpleData[i], decoded);
	}
}