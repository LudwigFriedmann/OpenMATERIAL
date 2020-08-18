//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      utils.cpp
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-03-24
/// @brief     Utility and helper functions

#include <cctype> // std::isspace
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#include "doctest.h"
#include "utils.h"

namespace utils
{
	using std::size_t;

	namespace path
	{

		/// File separator of the current OS
		const char separator =
		#ifdef _WIN32
					'\\';
		#else
					'/';
		#endif

		/// @brief Unit test for utils::path::basename
		TEST_CASE("Testing utils::path::basename")
		{
			CHECK(basename("") == "");
			std::string pathname = separator + std::string("usr") + separator + std::string("bin") + separator + "vim";
			CHECK(basename(pathname) == "vim");
			CHECK(basename(".foo") == ".foo");
		}

		/// @brief Unit test for utils::path::fileextension
		TEST_CASE("Testing utils::path::fileextension")
		{
			CHECK(fileextension("") == "");
			CHECK(fileextension("foo.bin") == "bin");
			CHECK(fileextension(".foo.bin") == "bin");
			CHECK(fileextension(".foo") == "");
		}

		/// @brief Unit test for utils::path::dirname
		TEST_CASE("Testing utils::path::dirname")
		{
			std::string input, output;

			input = separator + std::string("foo") + separator;
			output = separator + std::string("foo");
			CHECK(dirname(input) == output);

			input = separator + std::string("usr") + separator + std::string("bin") + separator + std::string("vim");
			output = separator + std::string("usr") + separator + std::string("bin");
			CHECK(dirname(input) == output);
		}

		/// File separator of the operating system
		char getFileSeparator()
		{
			return separator;
		}

		/// Return the final component of a pathname
		std::string basename(const std::string &pathname)
		{
			std::size_t pos = pathname.find_last_of(separator);
			if(pos == std::string::npos)
				return pathname;

			return pathname.substr(pos+1);
		}

		/// @brief Return the file extension of the file name
		///
		/// Note that files like .bashrc do not have a file extension and consequently
		/// this function returns "" for such path names.
		std::string fileextension(const std::string &pathname)
		{
			// Get basename
			std::string bname = basename(pathname);

			std::size_t start = bname.find_first_not_of('.');
			std::size_t pos = bname.find_last_of('.');

			if(start >= pos)
				return "";

			return bname.substr(pos+1);
		}

		/// Returns the directory component of a pathname
		std::string dirname(const std::string &rsPathname)
		{
			size_t pos = rsPathname.find_last_of(separator);
			return rsPathname.substr(0,pos);
		}

		/// Check if file given by crsFileName exists
		bool fileExists(const std::string &crsFileName)
		{
			std::ifstream f(crsFileName.c_str());
			return f.good();
		}
	}

	namespace string
	{

		/// @brief Unit test for utils::string::strcaseequal
		TEST_CASE("Testing utils::string::strcaseequal")
		{
			CHECK(strcaseequal("", ""));
			CHECK(strcaseequal("abc", "aBc"));
			CHECK(!strcaseequal("abc", "adc"));
		}

		/// @brief Unit test for utils::string::lstrip
		TEST_CASE("Testing utils::string::lstrip")
		{
			CHECK(lstrip("") == "");
			CHECK(lstrip("abc") == "abc");
			CHECK(lstrip(" ") == "");
			CHECK(lstrip("  ") == "");
			CHECK(lstrip("\r") == "");
			CHECK(lstrip("\n") == "");
			CHECK(lstrip("\t") == "");
			CHECK(lstrip("\t\r\n") == "");
			CHECK(lstrip("\t\r\nabc") == "abc");
			CHECK(lstrip("  abc\r\t\n ") == "abc\r\t\n ");
			CHECK(lstrip(" ξ ") == "ξ "); // check for unicode
		}

		/// @brief Unit test for utils::string::rstrip
		TEST_CASE("testing utils::string::rstrip")
		{
			CHECK(rstrip("") == "");
			CHECK(rstrip(" ") == "");
			CHECK(rstrip("  ") == "");
			CHECK(rstrip("\r") == "");
			CHECK(rstrip("\n") == "");
			CHECK(rstrip("\t") == "");
			CHECK(rstrip("\t\r\n") == "");
			CHECK(rstrip("abc\t\r\n") == "abc");
			CHECK(rstrip("  abc\r\t\n ") == "  abc");
			CHECK(rstrip(" ξ ") == " ξ"); // check for unicode
		}

		/// @brief Unit test for utils::string::strip
		TEST_CASE("testing utils::string::strip")
		{
			CHECK(strip("") == "");
			CHECK(strip(" ") == "");
			CHECK(strip("  ") == "");
			CHECK(strip("\r") == "");
			CHECK(strip("\n") == "");
			CHECK(strip("\t") == "");
			CHECK(strip("\t\r\n") == "");
			CHECK(strip("abc\t\r\n") == "abc");
			CHECK(strip("  abc\r\t\n ") == "abc");
			CHECK(strip(" ξ ") == "ξ"); // check for unicode
		}

		/// @brief Unit test for utils::string::split
		TEST_CASE("testing utils::string::split")
		{
			std::vector<std::string> v;

			v = split("a,b,c", ",");
			REQUIRE(v.size() == 3);
			CHECK(v[0] == "a");
			CHECK(v[1] == "b");
			CHECK(v[2] == "c");

			v = split("", ",");
			REQUIRE(v.size() == 1);
			CHECK(v[0] == "");

			v = split("abc", "");
			CHECK(v.size() == 0);

			v = split("a,b,c,d,e,f", ",", 2);
			REQUIRE(v.size() == 3);
			CHECK(v[0] == "a");
			CHECK(v[1] == "b");
			CHECK(v[2] == "c,d,e,f");
		}

		/// @brief Check if the strings a and b are equal ignoring the case of the characters
		///
		/// This function does not work for unicode strings.
		bool strcaseequal(const std::string& a, const std::string& b)
		{
			if (a.size() != b.size())
				return false;

			for(size_t i = 0; i < a.size(); i++)
				if (tolower(a[i]) != tolower(b[i]))
					return false;
			return true;
		}

		/// Return copy of string s with leading white space removed
		std::string lstrip(const std::string &s)
		{
			size_t start, length = s.length();
			for(start = 0; start < length && std::isspace(s[start]); start++);
			return s.substr(start,std::string::npos);
		}

		/// Return copy of string s with trailing white space removed
		std::string rstrip(const std::string &s)
		{
			size_t stop, length = s.length();
			for(stop = 0; stop < length && std::isspace(s[length-stop-1]); stop++);
			return s.substr(0, length-stop);
		}

		/// Return copy of string s with leading and trailing white space removed
		std::string strip(const std::string &s)
		{
			size_t start, stop, length = s.length();
			for(start = 0; start < length && std::isspace(s[start]); start++);
			for(stop = 0; stop < length && std::isspace(s[length-stop-1]); stop++);
			return s.substr(start,length-stop-start);
		}

		/// @brief Return a list of substrings in the string using sep as the delimiter
		///
		/// This function takes a string s and returns a vector of substrings, splitted
		/// by the delimiter given by delimiter. If delimiter is an empty string, an
		/// empty vector is returned.
		///
		/// @param [in] s string
		/// @param [in] delimiter delimiter according which to split the string
		/// @param [in] maxsplit Maximum number of splits (-1 means no limit)
		/// @retval substrings vector of substrings
		std::vector<std::string> split(const std::string &s, const std::string& delimiter, int maxsplit)
		{
			std::vector<std::string> output;

			if(delimiter.empty())
				return output;

			size_t pos_start = 0;

			for(int i = 0; true; i++)
			{
				size_t pos_stop;

				if(i == maxsplit)
					pos_stop = std::string::npos;
				else
					pos_stop = s.find(delimiter, pos_start);

				std::string token = s.substr(pos_start, pos_stop-pos_start);
				output.push_back(token);

				if(pos_stop == std::string::npos)
					break;

				pos_start = pos_stop+delimiter.length();
			}

			return output;
		}
	}
}