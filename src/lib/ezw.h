/**
 * @file ezw.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef EZW_H
#define EZW_H

#include <cstdlib>

class EzwCodec
{
protected:
	struct Element
	{
		enum class Code { Pos, Neg, IsolatedZero, ZeroTreeRoot };

		Element() {}
		Element(size_t x, size_t y) : x(x), y(y) { }

		size_t x, y;
		Code code;
	};

	EzwCodec() {}
};

#endif // !EZW_H
