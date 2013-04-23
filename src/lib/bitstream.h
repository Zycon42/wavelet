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

class BitStreamReader
{
public:
	explicit BitStreamReader(std::istream* stream) {
		reset(stream);
	}

	void reset(std::istream* stream) {
		byte = 0;
		mask = 0;
		this->stream = stream;
	}

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

class BitStreamWriter
{
public:
	explicit BitStreamWriter(std::ostream* stream) {
		reset(stream);
	}

	~BitStreamWriter() {
		flush();
	}

	void flush() {
		// when we have something in buffer, write whole byte to stream
		if (mask != 0x80)
			stream->put(byte);
	}

	void reset(std::ostream* stream) {
		byte = 0;
		mask = 0x80;
		this->stream = stream;
	}

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
