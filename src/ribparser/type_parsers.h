#ifndef PPRT_TYPE_PARSERS_H
#define PPRT_TYPE_PARSERS_H

#include <boost/spirit/include/classic_functor_parser.hpp>

#include <cmath>
#include <limits>

namespace tools
{
	//! Extract a number from a scanner
	template<typename ScannerT>
	int	scanNumber(ScannerT const &scan, int &out, bool *negative = 0)
	{
		if (scan.at_end())
			return -1;

		int matchedLen = 0;
		char ch = *scan;

		// check sign
		if (negative)
			*negative = false;
		if (!scan.at_end() && (ch = *scan, ch == '-'))
		{
			++scan;
			++matchedLen;
			ch = *scan;
			if (negative)
				*negative = true;
		}

		if (ch < '0' || ch > '9')
			return -1;

		out = 0;
		do
		{
			out = out*10 + int(ch - '0');
			++scan;
			++matchedLen;
		} while (!scan.at_end() && (ch = *scan, ch >= '0' && ch <= '9'));

		return matchedLen;
	}

	//! Extract a float from a scanner
	template<typename ScannerT, typename floatT>
	int	scanFloat(ScannerT const &scan, floatT &out, float bounding = false,
				  float minBound = 0, float maxBound = 0)
	{
		if (scan.at_end())
			return -1;

		int matchedLen = 0;
		int matched;
		bool negative;

		// Scan integer part
		int int_part;
		matched = scanNumber(scan, int_part, &negative);
		if (matched == -1)
			return -1;
		matchedLen += matched;
		out = float(int_part);

		// Check if we reached a separator (comma)
		if (scan.at_end())
			return -1;
		if (*scan == ',')
		{
			// Check min/max boundary
			if (bounding && (out < minBound || out > maxBound))
				return -1;

			++scan;
			++matchedLen;
			if (negative)
				out = -out;

			return matchedLen;
		}

		// Check if dot is here, if not, assume the float is made only of its integer part.
		if (scan.at_end())
			return -1;

		char ch = *scan;
		if (ch != '.')
		{
			// Check min/max boundary
			if (bounding && (out < minBound || out > maxBound))
				return -1;

			if (negative)
				out = -out;

			return matchedLen;
		}
		++scan;

		// Scan fractional part
		int frac_part;
		float frac_length;
		frac_length = (float)scanNumber(scan, frac_part);
		matched += (int)frac_length;
		if (matched == -1 || frac_part < 0)
			return -1;

		matchedLen += matched;

		// Compose the result
		frac_length = pow(10.0f, frac_length);
		if (frac_length != 0 && frac_part != 0)
			out += frac_part / frac_length;

		// Check min/max boundary
		if (bounding && (out < minBound || out > maxBound))
			return -1;

		if (negative)
			out = -out;

		return matchedLen;
	}

	//! Eat whitespaces
	template<typename ScannerT>
	int eatWhitespaces(ScannerT const &scan)
	{
		if (scan.at_end())
			return -1;

		char ch;
		int matchedLen		= 0;

		while (!scan.at_end() && (ch = *scan, ch == ' '))
		{
			++matchedLen;
			++scan;
		}

		return matchedLen;
	}
}

struct bool_parser
{
	typedef bool	result_t;

	//! \todo throw malformed exception
	template <typename ScannerT>
	int operator()(ScannerT const &scan, result_t &result) const
	{
		if (scan.at_end())
			return -1;

		int len = 0;
		char ch = *scan;

		int matched = tools::eatWhitespaces(scan);
		if (matched == -1)
			return -1;
		len += matched;

		int res;
		matched = tools::scanNumber(scan, res, 0);
		if (matched == -1)
			return -1;
		if (res != 0 && res != 1)
			return -1;

		len += matched;

		result = result_t(res == 1);

		return len;
	}
};
extern boost::spirit::classic::functor_parser<bool_parser> bool_p;

//! Build a Color out of the scanner
struct color_parser
{
	typedef NonAlignedVec3		result_t;

	//! \todo throw malformed exception
	template <typename ScannerT>
	int operator()(ScannerT const &scan, result_t &result) const
	{
		if (scan.at_end())
			return -1;

		int len = 0;
		char ch = *scan;

		int matched = tools::eatWhitespaces(scan);
		if (matched == -1)
			return -1;
		len += matched;

		float res[3];
		for (int i = 0; i < 3; i++)
		{
			matched = tools::scanFloat(scan, res[i], 0, 1);
			if (matched == -1)
				return -1;
			len += matched;
			
			// Don't remove last space to allow for the blank_p rule to parse as expected
			if (i != 2)
			{
				matched = tools::eatWhitespaces(scan);
				if (matched == -1)
					return -1;
				len += matched;
			}
		}

		result = result_t(res[0], res[1], res[2]);

		return len;
	}
};
extern boost::spirit::classic::functor_parser<color_parser> color_p;

//! Build a Vec3 out of the scanner
struct vec3_parser
{
	typedef NonAlignedVec3		result_t;

	//! \todo throw malformed exception
	template <typename ScannerT>
	int operator()(ScannerT const &scan, result_t &result) const
	{
		if (scan.at_end())
			return -1;

		int len = 0;
		char ch = *scan;

		int matched = tools::eatWhitespaces(scan);
		if (matched == -1)
			return -1;
		len += matched;

		float res[3];
		for (int i = 0; i < 3; i++)
		{
			matched = tools::scanFloat(scan, res[i], 0, 1);
			if (matched == -1)
				return -1;
			len += matched;

			// Don't remove last space to allow for the blank_p rule to parse as expected
			if (i != 2)
			{
				matched = tools::eatWhitespaces(scan);
				if (matched == -1)
					return -1;
				len += matched;
			}
		}

		result = result_t(res[0], res[1], res[2]);

		return len;
	}
};
extern boost::spirit::classic::functor_parser<vec3_parser> vec3_p;

#endif