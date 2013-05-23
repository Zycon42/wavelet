/**
 * @file bitstream.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <iostream>
#include <cstdint>
#include <stdexcept>

/**
 * Reader for individual bits from stl streams.
 */
class BitStreamReader
{
public:
	/**
	 * Constructs new reader from stream.
	 * @param stream stl istream
	 */
	explicit BitStreamReader(std::istream* stream) {
		reset(stream);
	}

	/**
	 * Resets reader to work on new stream.
	 * @param stream new istream to read from
	 */
	void reset(std::istream* stream) {
		byte = 0;
		mask = 0;
		this->stream = stream;
	}

	/**
	 * Read single bit from stream.
	 * @return true if read bit set false otherwise
	 * @throws std::runtime_error when unable to read from stream
	 */
	bool readBit() {
		if (mask == 0) {
			if (!stream->read(reinterpret_cast<char*>(&byte), 1))
				throw std::runtime_error("Unable to read from stream!");
			mask = 0x80;
		}

		bool bit = !!(byte & mask);
		mask >>= 1;
		return bit;
	}
private:
	std::istream* stream;

	uint8_t byte;		/// buffer for current byte
	uint8_t mask;
};

/**
 * Writer for individual bits to stl streams.
 */
class BitStreamWriter
{
public:
	/**
	 * Constructs new writer for stream.
	 * @param stream stl ostream
	 */
	explicit BitStreamWriter(std::ostream* stream) {
		reset(stream);
	}

	~BitStreamWriter() {
		flush();
	}

	/**
	 * Flushes internal writing buffer.
	 */
	void flush() {
		// when we have something in buffer, write whole byte to stream
		if (mask != 0x80) {
			stream->put(byte);
			byte = 0;
			mask = 0x80;
		}
	}

	/**
	 * Resets reader to work on new stream.
	 * @param stream new ostream to write to
	 */
	void reset(std::ostream* stream) {
		byte = 0;
		mask = 0x80;
		this->stream = stream;
	}

	/**
	 * Writes single bit to stream.
	 * @param bit true if new bit should be set, false otherwise
	 * @throws std::runtime_error when failed to write to stream
	 */
	void writeBit(bool bit) {
		// set bit if we should
		if (bit)
			byte |= mask;

		mask >>= 1;
		if (mask == 0) {
			if (!stream->put(byte))
				throw std::runtime_error("Unable to put byte into stream!");

			byte = 0;
			mask = 0x80;
		}
	}
private:
	std::ostream* stream;

	uint8_t byte;
	uint8_t mask;
};

#endif // !BITSTREAM_H
