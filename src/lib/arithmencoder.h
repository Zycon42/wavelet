/**
 * @file arithmencoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef ARITHMENCODER_H
#define ARITHMENCODER_H

#include "arithmcodec.h"
#include "bitstream.h"

#include <memory>

/**
 * Encoder for arithmetic coding.
 * 32bit integer implementation.
 * @see http://phoenix.inf.upol.cz/esf/ucebni/komprese.pdf
 */
class ArithmeticEncoder
{
public:
	/// Ctor
	explicit ArithmeticEncoder(std::shared_ptr<BitStreamWriter>& bsw);

	~ArithmeticEncoder() {
		close();
	}

	void reset();

	/**
	 * Finishes encoding, writing last necessary bits.
	 */
	void close();

	/**
	 * Encodes given symbol with data model.
	 * Result will be stored to data buffer accessible with {@link data} method,
	 * also {@link writtenBits} counter will be increased by number of bits
	 * required to store encoded symbol.
	 * @param symbol symbol from dataModel to encode
	 * @param dataModel data model with frequencies
	 */
	void encode(unsigned symbol, DataModel* dataModel);

	std::shared_ptr<BitStreamWriter> writer() {
		return bitStreamWriter;
	}
private:
	typedef IntervalTraits<sizeof(uint32_t)> IntervalTraitsType;

	void encodeIntervalChange(bool flag);

	std::shared_ptr<BitStreamWriter> bitStreamWriter;

	uint64_t intervalLow;			/// lower interval bound
	uint64_t intervalHigh;			/// upper interval bound
	/// counter that counts how many times in row was interval enlarged from
	/// middle possible range
	std::size_t counter;
};

#endif // !ARITHMENCODER_H
