/**
 * @file arithmdecoder.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "arithmdecoder.h"

ArithmeticDecoder::ArithmeticDecoder(std::shared_ptr<BitStreamReader>& bsr) : bitStreamReader(bsr),
	intervalLow(0), intervalHigh(IntervalTraitsType::MAX)  {
		
	// read first IntervalTraitsType::BITS from data to value
	value = 0;
	for (size_t i = 0; i < IntervalTraitsType::BITS; ++i)
		readBit();
}

void ArithmeticDecoder::reset() {
	intervalLow = 0;
	intervalHigh = IntervalTraitsType::MAX;

	// read first IntervalTraitsType::BITS from data to value
	value = 0;
	for (size_t i = 0; i < IntervalTraitsType::BITS; ++i)
		readBit();
}

void ArithmeticDecoder::readBit() {
	bool bit;
	try {
		bit = bitStreamReader->readBit();
	} catch (std::exception&) {
		bit = false;		// on data end we append zero bit
	}

	value <<= 1;
	if (bit)
		value += 1;
}

unsigned ArithmeticDecoder::decode(DataModel* dataModel) {
	uint64_t range = intervalHigh - intervalLow + 1;
	auto scale = dataModel->getCumulativeFreq(dataModel->size() - 1);
	auto cumulativeFreq = ((value - intervalLow + 1) * scale - 1) / range;

	unsigned symbol = 0;
	// find i where cumuliveFreqs[i-1] < cumulativeFreq < cumulativeFreqs[i]
	for (size_t i = 0; i < dataModel->size(); ++i) {
		auto lowerBound = i != 0 ? dataModel->getCumulativeFreq(i - 1) : 0U;
		auto upperBound = dataModel->getCumulativeFreq(i);
		if (lowerBound <= cumulativeFreq && cumulativeFreq < upperBound) {
			symbol = i;
			break;
		}
	}

	// compute new interval bounds

	// interval upper bound is
	intervalHigh = intervalLow + (range * dataModel->getCumulativeFreq(symbol)) / scale - 1;

	// interval lower bound is computed as low + (r * cumFreq(i-1)) / s and cumFreq(-1) == 0 so
	// if symbol == 0 then interval lower bound is not modified 
	if (symbol != 0) {
		intervalLow += (range * dataModel->getCumulativeFreq(symbol - 1)) / scale;
	}

	// enlarge interval and get bits from data
	// loop ends when interval is large enough
	for (;;) {
		// interval is in lower half of possible range
		if (intervalHigh < IntervalTraitsType::HALF) {
			;	// do nothing
		// interval is in upper half of possible range
		} else if (intervalLow >= IntervalTraitsType::HALF) {
			intervalLow -= IntervalTraitsType::HALF;
			intervalHigh -= IntervalTraitsType::HALF;
			value -= IntervalTraitsType::HALF;
		// interval is in middle of possible range
		} else if (intervalLow >= IntervalTraitsType::QUARTER && intervalHigh <= IntervalTraitsType::THREE_QUARTERS) {
			intervalLow -= IntervalTraitsType::QUARTER;
			intervalHigh -= IntervalTraitsType::QUARTER;
			value -= IntervalTraitsType::QUARTER;
		// none of these cases so break loop
		} else
			break;

		intervalLow <<= 1;
		intervalHigh = (intervalHigh << 1) + 1;
		readBit();
	}

	// on adaptive data model we increase symbol frequency
	auto adaptiveModel = dynamic_cast<AdaptiveDataModel*>(dataModel);
	if (adaptiveModel != nullptr)
		adaptiveModel->incSymbolFreq(symbol);

	return symbol;
}