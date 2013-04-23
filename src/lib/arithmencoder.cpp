/**
 * @file arithmencoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "arithmencoder.h"

ArithmeticEncoder::ArithmeticEncoder(std::shared_ptr<BitStreamWriter>& bsw) : bitStreamWriter(bsw), 
	intervalLow(0), intervalHigh(IntervalTraitsType::MAX), counter(0) { }

void ArithmeticEncoder::close() {
	counter++;
	if (intervalLow < IntervalTraitsType::QUARTER) {
		encodeIntervalChange(false);
	} else {
		encodeIntervalChange(true);
	}

	bitStreamWriter->flush();
}

void ArithmeticEncoder::reset() {
	close();

	intervalLow = 0;
	intervalHigh = IntervalTraitsType::MAX;
	counter = 0;
}

void ArithmeticEncoder::encodeIntervalChange(bool flag) {
	bitStreamWriter->writeBit(flag);
	// handle third case, we use relation that (C3)^k C1 = C1 (C2)^k
	for (; counter > 0; --counter)
		bitStreamWriter->writeBit(!flag);
}

void ArithmeticEncoder::encode(unsigned symbol, DataModel* dataModel) {
	// compute helper value
	auto range = intervalHigh - intervalLow + 1;
	auto scale = dataModel->getCumulativeFreq(dataModel->size() - 1);

	// interval upper bound
	intervalHigh = intervalLow + (range * dataModel->getCumulativeFreq(symbol)) / scale - 1;

	// interval lower bound is computed as low + (r * cumFreq(i-1)) / s and cumFreq(-1) == 0 so
	// if symbol == 0 then interval lower bound is not modified 
	if (symbol != 0) {
		intervalLow += (range * dataModel->getCumulativeFreq(symbol - 1)) / scale;
	}

	// enlarge interval and send info about it to output
	// loop ends when interval is large enough
	for (;;) {
		// interval is in lower half of possible range
		if (intervalHigh < IntervalTraitsType::HALF) {
			// encode first case as zero
			encodeIntervalChange(false);

		// interval is in upper half of possible range
		} else if (intervalLow >= IntervalTraitsType::HALF) {
			// enlarge interval
			intervalLow -= IntervalTraitsType::HALF;
			intervalHigh -= IntervalTraitsType::HALF;

			// encode second case as one
			encodeIntervalChange(true);

		// interval is in middle of possible range
		} else if (intervalLow >= IntervalTraitsType::QUARTER && intervalHigh < IntervalTraitsType::THREE_QUARTERS) {
			// enlarge interval
			intervalLow -= IntervalTraitsType::QUARTER;
			intervalHigh -= IntervalTraitsType::QUARTER;

			// this case can't be encoded directly but we can prove that
			// (C3)^k C1 = C1 (C2)^k and (C3)^k C2 = C2 (C1)^k so we count
			// third cases in row and than handle them in first and second case using
			// above formulas

			// increase counter
			counter++;

		// none of these cases so break loop
		} else
			break;

		intervalLow <<= 1;
		intervalHigh = (intervalHigh << 1) + 1;
	}

	// on adaptive data model we increase symbol frequency
	auto adaptiveModel = dynamic_cast<AdaptiveDataModel*>(dataModel);
	if (adaptiveModel != nullptr)
		adaptiveModel->incSymbolFreq(symbol);
}
