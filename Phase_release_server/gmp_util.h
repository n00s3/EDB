#pragma once
/*
	Programmed By Thomas Kim
	August 6, 2017
*/

#ifndef _GMP_UTIL_H
#define _GMP_UTIL_H

#include <iostream>
#include <string>

#pragma comment(lib, "mpir.lib")

#pragma warning(push)
#pragma warning(disable: 4244)

#include <mpirxx.h>

#pragma warning(pop)

#pragma comment(lib, "mpfr.lib")

#include <mpreal.h>

inline std::ostream& operator<<(std::ostream& os, const mpz_class& z)
{
	std::string s = z.get_str();
	int j, size = (int)s.size();
	int mode = size % 3;

	for (int i = 0; i < size; ++i)
	{
		os << s[i];

		j = i + 1;
		if (j % 3 == mode && j != size)
			os << ',';
	}
	return os;
}

inline std::wostream& operator<<(std::wostream& os, const mpz_class& z)
{
	std::string s = z.get_str();
	int j, size = (int)s.size();
	int mode = size % 3;

	for (int i = 0; i < size; ++i)
	{
		os << (wchar_t)s[i];

		j = i + 1;
		if (j % 3 == mode && j != size)
			os << ',';
	}
	return os;
}

#endif _GMP_UTIL_H
