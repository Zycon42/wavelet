/**
 * @file arithmdecoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef ARITHMDECODER_H
#define ARITHMDECODER_H

#include "arithmcodec.h"
#include "bitstream.h"

#include <memory>

class ArithmeticDecoder
{
public:
	explicit ArithmeticDecoder(std::shared_ptr<BitStreamReader>& bsr);

	void reset();

	unsigned decode(DataModel* dataModel);

	std::shared_ptr<BitStreamReader> reader() {
		return bitStreamReader;
	}
private:
	typedef IntervalTraits<sizeof(uint32_t)> IntervalTraitsType;

	void readBit();

	std::shared_ptr<BitStreamReader> bitStreamReader;

	uint64_t intervalLow;			/// lower interval bound
	uint64_t intervalHigh;			/// upper interval bound
	uint64_t value;
};

#endif // !ARITHMDECODER_H
