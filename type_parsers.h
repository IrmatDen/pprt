#ifndef CRT_TYPE_PARSERS_H
#define CRT_TYPE_PARSERS_H

// IMPORTANT: this file can't be used alone, it's only to be included in scnparser.h which
// contains the good includes and typedefs!

#include <cmath>
#include <limits>

#undef min
#undef max

namespace tools
{
	//! Extract a number from a scanner
	template<typename ScannerT>
	int	scanNumber(ScannerT const &scan, int &out)
	{
		if (scan.at_end())
			return -1;

		int matchedLen = 0;
		char ch = *scan;

		// check sign
		bool negative = false;
		if (!scan.at_end() && (ch = *scan, ch == '-'))
		{
			++scan;
			++matchedLen;
			ch = *scan;
			negative = true;
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

		if (negative)
			out = -out;

		return matchedLen;
	}

	//! Extract a float from a scanner
	template<typename ScannerT>
	int	scanFloat(ScannerT const &scan, float &out,
				  float minBound = std::numeric_limits<float>::min(),
				  float maxBound = std::numeric_limits<float>::max())
	{
		if (scan.at_end())
			return -1;

		int matchedLen = 0;
		int matched;

		// Scan integer part
		int int_part;
		matched = scanNumber(scan, int_part);
		if (matched == -1)
			return -1;
		matchedLen += matched;
		out = float(int_part);

		// Check if we reached the a separator (comma)
		if (scan.at_end())
			return -1;
		if (*scan == ',')
		{
			++scan;
			++matchedLen;
			return matchedLen;
		}

		// Check if dot is here, if not, assume the float is made only of its integer part.
		if (scan.at_end())
			return -1;

		char ch = *scan;
		if (ch != '.')
			return matchedLen;
		scan++;

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
		if (out < minBound || out > maxBound)
			return -1;

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

		float res[4];
		for (int i = 0; i < 4; i++)
		{
			int matched = tools::scanFloat(scan, res[i], 0, 1);
			if (matched == -1)
				return -1;
			len += matched;
			matched = tools::eatSeparator(scan);
			if (matched == -1)
				return -1;
			len += matched;
		}

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

//! Build a Vec3f out of the scanner
struct vec3f_parser
{
	typedef Vec3f		result_t;

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

		float res[3];
		for (int i = 0; i < 3; i++)
		{
			int matched = tools::scanFloat(scan, res[i], 0, 1);
			if (matched == -1)
				return -1;
			len += matched;
			matched = tools::eatSeparator(scan);
			if (matched == -1)
				return -1;
			len += matched;
		}

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
functor_parser<vec3f_parser> vec3f_p;

#endif
