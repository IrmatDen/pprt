#ifndef CRT_TYPE_PARSERS_H
#define CRT_TYPE_PARSERS_H

// IMPORTANT: this file can't be used alone, it's only to be included in scnparser.h which
// contains the good includes and typedefs!

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
	template<typename ScannerT>
	int	scanFloat(ScannerT const &scan, float &out, float bounding = false,
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
		matched = scanNumber(scan, frac_part);
		if (matched == -1 || frac_part < 0)
			return -1;

		matchedLen += matched;

		// Compose the result
		int decal = 10 + (int)log10((float)frac_part) * 10;
		if (decal != 0 && frac_part != 0)
			out += float(frac_part) / decal;

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

	//! Eat all whitespaces and comma
	template<typename ScannerT>
	int eatSeparator(ScannerT const &scan)
	{
		if (scan.at_end())
			return -1;

		char ch;
		int matchedLen		= 0;
		bool commaSpotted	= false;

		while (!scan.at_end() && (ch = *scan, ch == ',' || ch == ' '))
		{
			if (ch == ',')
			{
				if (!commaSpotted)	commaSpotted = true;
				else				return -1;
			}

			++matchedLen;
			++scan;
		}

		return matchedLen;
	}
}

//! Build a Color4 out of the scanner
struct color4_parser
{
	typedef Color4		result_t;

	//! \todo throw malformed exception
	template <typename ScannerT>
	int operator()(ScannerT const &scan, result_t &result) const
	{
		if (scan.at_end())
			return -1;

		int len = 0;
		char ch = *scan;
		if (ch != '(')
			return -1;
		++scan;
		++len;

		int matched = tools::eatWhitespaces(scan);
		if (matched == -1)
			return -1;
		len += matched;

		float res[4];
		for (int i = 0; i < 4; i++)
		{
			matched = tools::scanFloat(scan, res[i], 0, 1);
			if (matched == -1)
				return -1;
			len += matched;
			matched = tools::eatSeparator(scan);
			if (matched == -1)
				return -1;
			len += matched;
		}

		matched = tools::eatWhitespaces(scan);
		if (matched == -1)
			return -1;
		len += matched;

		if (scan.at_end() || *scan != ')')
			return -1;
		++scan;
		++len;

		result.r = res[0];
		result.g = res[1];
		result.b = res[2];
		result.a = res[3];

		return len;
	}
};
functor_parser<color4_parser> color4_p;

//! Build a Vec3d out of the scanner
struct Vec3d_parser
{
	typedef Vec3d		result_t;

	//! \todo throw malformed exception
	template <typename ScannerT>
	int operator()(ScannerT const &scan, result_t &result) const
	{
		if (scan.at_end())
			return -1;

		int len = 0;
		char ch = *scan;
		if (ch != '<')
			return -1;
		++scan;
		++len;

		int matched = tools::eatWhitespaces(scan);
		if (matched == -1)
			return -1;
		len += matched;

		float res[3];
		for (int i = 0; i < 3; i++)
		{
			int matched = tools::scanFloat(scan, res[i]);
			if (matched == -1)
				return -1;
			len += matched;
			matched = tools::eatSeparator(scan);
			if (matched == -1)
				return -1;
			len += matched;
		}

		matched = tools::eatWhitespaces(scan);
		if (matched == -1)
			return -1;
		len += matched;

		if (scan.at_end() || *scan != '>')
			return -1;
		++scan;
		++len;

		result.x = res[0];
		result.y = res[1];
		result.z = res[2];

		return len;
	}
};
functor_parser<Vec3d_parser> Vec3d_p;

#endif
