//
//  This is a program that generates srell_ucfdata.hpp from CaseFolding.txt
//  provided by the Unicode Consortium. The latese version is available at:
//  http://www.unicode.org/Public/UNIDATA/CaseFolding.txt
//
//  ucfdataout.cpp: version 1.001 (2014/11/01).
//

#include <cstdio>
#include <string>
#include "../srell.hpp"

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable:4996)
#endif

bool read_file(std::string &str, const char *const filename)
{
	FILE *const fp = ::fopen(filename, "r");

	::fprintf(stdout, "Reading '%s'... ", filename);

	if (fp)
	{
		static const std::size_t bufsize = 1024;
		char buffer[bufsize];

		for (;;)
		{
			const std::size_t size = ::fread(buffer, 1, bufsize, fp);

			if (!size)
				break;

			str.append(buffer, size);
		}
		::fclose(fp);
		::fputs("done.\n", stdout);
		return true;
	}
	::fputs("error!\n", stdout);
	return false;
}


int main(const int argc, const char *const *const argv)
{
	const char *infilename = "CaseFolding.txt";
	const char *outfilename = "srell_ucfdata.hpp";
	const char *const indent = "\t\t\t";
	std::string buf;

	if (argc > 1)
		infilename = argv[1];

	if (argc > 2)
		outfilename = argv[2];

	if (read_file(buf, infilename))
	{
		FILE *const fp = ::fopen(outfilename, "w");

		if (fp)
		{
			const srell::regex re("^.*$", srell::regex::multiline);
			const srell::cregex_iterator eos;
			srell::cregex_iterator iter(buf.c_str(), buf.c_str() + buf.size(), re);
			srell::cmatch match;
			long maxdelta = 0L;
			long maxdeltacp = 0L;
			long maxcodepoint = 0L;

			for (; iter != eos; ++iter)
			{
				if (iter->length(0))
				{
					const srell::regex re("^# (.*)$");

					if (!srell::regex_match((*iter)[0].first, (*iter)[0].second, match, re))
						break;

					::fprintf(fp, "//  %s\n", match.str(1).c_str());
				}
			}

			::fputs("template <typename T1, typename T2>\nstruct unicode_casefolding\n{\n\tstatic const T1 *table()\n\t{\n\t\tstatic const T1 ucftable[] =\n\t\t{\n", fp);

			for (; iter != eos; ++iter)
			{
				const srell::regex re("^\\s*([0-9A-Fa-f]+); ([CS]); ([0-9A-Fa-f]+);\\s*#\\s*(.*)$");
				const srell::cmatch &line = *iter;

				if (srell::regex_match(line[0].first, line[0].second, match, re))
				{
					const std::string from(match[1]);
					const std::string to(match[3]);
					const std::string type(match[2]);
					const std::string name(match[4]);
					const long from_cp = ::strtol(from.c_str(), NULL, 16);
					const long to_cp = ::strtol(to.c_str(), NULL, 16);
					const long delta = to_cp - from_cp;

					if (::abs(maxdelta) < ::abs(delta))
					{
						maxdeltacp = from_cp;
						maxdelta = delta;
					}
					if (maxcodepoint < from_cp)
						maxcodepoint = from_cp;

					::fprintf(fp, "%s{ 0x%s, 0x%s },\t//  %s; %s\n", indent, from.c_str(), to.c_str(), type.c_str(), name.c_str());
				}
				else if (!srell::regex_match(line[0].first, line[0].second, srell::regex("(?:^#.*|^$)")))
					::fprintf(fp, "%s//  %s\n", indent, line.str(0).c_str());
			}
			::fprintf(fp, "%s{ 0, 0 }\n\t\t};\n\t\treturn ucftable;\n\t}\n", indent);
			::fprintf(fp, "\tstatic const T2 maxcodepoint = 0x%.6lx;\n};\n", maxcodepoint);
			::fclose(fp);

			::fprintf(stdout, "MaxDelta: %+ld (U+%.4lX->U+%.4lX)\n", maxdelta, maxdeltacp, maxdeltacp + maxdelta);
		}
		else
			::fprintf(stdout, "Could not open '%s' ...", outfilename);
	}
	return 0;
}
