//
//  A sample program for SRELL (test and benchmark).
//
//  If you use this sample with std::regex:
//    1. replace the include file "../srell.hpp" with std::regex,
//    2. replace "srell::" with "std::" in this source file.
//
//  If you use this sample with boost::regex:
//    1. replace the include file "../srell.hpp" with boost::regex,
//    2. replace "srell::" with "boost::" in this source file,
//    3. pass "boost::regex_constants::match_not_dot_newline" to
//       regex_search() as the fifth argument.
//
//  If you use this sample with boost::xpressive:
//    1. replace the include file "../srell.hpp" with boost::xpressive,
//    2. replace "srell::regex re" with "boost::xpressive::cregex re" in
//       this source file,
//    3. replace
//        "re.assign(exp, srell::regex::ECMAScript)"
//        with
//        "re = boost::xpressive::cregex::compile(exp, boost::xpressive::cregex::ECMAScript | boost::xpressive::cregex::not_dot_newline)" in this source file,
//    4. replace "srell::" with "boost::xpressive::" in this source file.
//

#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include "../srell.hpp"

void test(const std::string &str, const std::string &exp, const unsigned int max = 1)
{
	srell::regex re;
	srell::cmatch mr;
	bool b = false;

	try
	{
		re.assign(exp, srell::regex::ECMAScript);

		const clock_t st = ::clock();

		for (unsigned int i = 0; i < max; i++)
			b = srell::regex_search(str.c_str(), str.c_str() + str.size(), mr, re);

		const clock_t ed = ::clock();

		::fprintf(stdout, "\t\"%s\" =~ /%s/\n\tx %u\n", str.c_str(), exp.c_str(), max);
		::fprintf(stdout, "\t%s (%ld msec)\n", b ? "Found" : "Not Found", static_cast<long>(static_cast<double>(ed - st) * 1000 / CLOCKS_PER_SEC));

		for (srell::cmatch::size_type i = 0; i < mr.size(); ++i)
		{
			::fprintf(stdout, "\t%.2u: ", i);
			if (mr[i].matched)
				::fprintf(stdout, "\"%s\" (%u-%u)\n", mr[i].str().c_str(), mr.position(i), mr.length(i));
			else
				::fputs("(NULL)\n", stdout);
		}
		::fputs("\n", stdout);
	}
	catch (const srell::regex_error &e)
	{
		::fprintf(stdout, "Error: %d \"%s\"\n", e.code(), e.what());
	}
	catch (const std::exception &e)
	{
		::fprintf(stdout, "Error: \"%s\"\n", e.what());
	}
}


int main()
{
	const unsigned int count = 100000;
	std::string exp;
	std::string str;

	::fputs("Test 1 (ECMAScript specification 15.10.2.5, note 2)\n", stdout);
	str = "abcdefghi";
	exp = "a[a-z]{2,4}";
	test(str, exp);

	::fputs("Test 2 (ECMAScript specification 15.10.2.5, note 2)\n", stdout);
	str = "abcdefghi";
	exp = "a[a-z]{2,4}?";
	test(str, exp);

	::fputs("Test 3 (ECMAScript specification 15.10.2.5, note 2)\n", stdout);
	str = "aabaac";
	exp = "(aa|aabaac|ba|b|c)*";
	test(str, exp);

	::fputs("Test 4 (ECMAScript specification 15.10.2.5, note 3)\n", stdout);
	str = "zaacbbbcac";
	exp = "(z)((a+)?(b+)?(c))*";
	test(str, exp);

	::fputs("Test 5 (ECMAScript specification 15.10.2.5, note 4)\n", stdout);
	str = "b";
	exp = "(a*)*";
	test(str, exp);

	::fputs("Test 6 (ECMAScript specification 15.10.2.5, note 4)\n", stdout);
	str = "baaaac";
	exp = "(a*)b\\1+";
	test(str, exp);

	::fputs("Test 7 (ECMAScript specification 15.10.2.8, note 2)\n", stdout);
	str = "baaabac";
	exp = "(?=(a+))";
	test(str, exp);

	::fputs("Test 8 (ECMAScript specification 15.10.2.8, note 2)\n", stdout);
	str = "baaabac";
	exp = "(?=(a+))a*b\\1";
	test(str, exp);

	::fputs("Test 9 (ECMAScript specification 15.10.2.8, note 3)\n", stdout);
	str = "baaabaac";
	exp = "(.*?)a(?!(a+)b\\2c)\\2(.*)";
	test(str, exp);


	::fputs("Bench 01\n", stdout);
	     //0123456
	str = "aaaabaa";
	exp = "^(.*)*b\\1$";
	test(str, exp, count);


	::fputs("Bench 02\n", stdout);
	     //0123456
	str = "aaaabaaaa";
	exp = "^(.*)*b\\1\\1$";
	test(str, exp, count);


	::fputs("Bench 03\n", stdout);
	     //01
	str = "ab";
	exp = "(.*?)*b\\1";
	test(str, exp, count * 10);


	::fputs("Bench 04\n", stdout);
	     //01234567
	str = "acaaabbb";
	exp = "(a(.)a|\\2(.)b){2}";
	test(str, exp, count * 10);


	::fputs("Bench 05\n", stdout);
	str = "aabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaaa";
	exp = "(a*)(b)*\\1\\1\\1";
	test(str, exp, count);


	::fputs("Bench 06\n", stdout);
	str = "aaaaaaaaaab";
	exp = "(.*)*b";
	test(str, exp, count * 10);

	::fputs("Bench 06a\n", stdout);
	str = "aaaaaaaaaab";
	exp = "(.*)+b";
	test(str, exp, count * 10);

	::fputs("Bench 06b\n", stdout);
	str = "aaaaaaaaaab";
	exp = "(.*){2,}b";
	test(str, exp, count * 10);


	::fputs("Bench 07\n", stdout);
	str = "aaaaaaaaaabc";
	exp = "(?=(a+))(abc)";
	test(str, exp, count);


	::fputs("Bench 08\n", stdout);
	str = "1234-5678-1234-456";
	exp = "(\\d{4}[-]){3}\\d{3,4}";
	test(str, exp, count * 5);


	::fputs("Bench 09\n", stdout);
	str = "aaaaaaaaaaaaaaaaaaaaa";
	exp = "(.*)*b";
	test(str, exp);

return 0;

	::fputs("Bench 10\n", stdout);
	str = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxz";
	exp = "(x+y*)+a";
	test(str, exp);

	return 0;
}
