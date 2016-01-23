//
//  ucfdataout.cpp: version 1.100a (2015/04/29).
//
//  This is a program that generates srell_ucfdata.hpp from CaseFolding.txt
//  provided by the Unicode Consortium. The latese version is available at:
//  http://www.unicode.org/Public/UNIDATA/CaseFolding.txt
//

#include <cstdio>
#include <string>
#include <map>
#include "../srell.hpp"

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable:4996)
#endif

class ucf_data
{
public:

	long maxdelta;	//  = 0L;
	long maxdelta_cp;	//  = 0L;
	long ucf_maxcodepoint;	//  = 0L;	//  the max code point for case-folding.
	long rev_maxcodepoint;	//  = 0L;	//  the max code point for reverse lookup.
	unsigned int ucf_numofsegs;	//  = 1;	//  the number of segments in the delta table.
	unsigned int rev_numofsegs;	//  = 1;	//  the number of segments in the table for reverse lookup.
	unsigned int numofcps_from;	//  = 0;	//  the number of code points in "folded from"s.
	unsigned int numofcps_to;	//  = 0;	//  the number of code points in "folded to"s.

	ucf_data()
		: maxdelta(0L), maxdelta_cp(0L), ucf_maxcodepoint(0L), rev_maxcodepoint(0L)
		, ucf_numofsegs(1), rev_numofsegs(1), numofcps_from(0), numofcps_to(0)
	{
	}

	void update(const std::string &from, const std::string &to)
	{
		const long cp_from = std::strtol(from.c_str(), NULL, 16);
		const long cp_to = std::strtol(to.c_str(), NULL, 16);
		const long delta = cp_to - cp_from;
		const long segno_from = cp_from >> 8;
		const long segno_to = cp_to >> 8;

		++numofcps_from;
		if (std::abs(maxdelta) < std::abs(delta))
		{
			maxdelta_cp = cp_from;
			maxdelta = delta;
		}

		if (ucf_maxcodepoint < cp_from)
			ucf_maxcodepoint = cp_from;

		if (rev_maxcodepoint < cp_to)
			rev_maxcodepoint = cp_to;

		if (rev_maxcodepoint < cp_from)
			rev_maxcodepoint = cp_from;

		if (!ucf_countedsegnos.count(segno_from))
		{
			ucf_countedsegnos[segno_from] = 1;
			++ucf_numofsegs;
		}

		if (!rev_countedsegnos.count(segno_to))
		{
			rev_countedsegnos[segno_to] = 1;
			++rev_numofsegs;
		}
		if (!rev_countedsegnos.count(segno_from))
		{
			rev_countedsegnos[segno_from] = 1;
			++rev_numofsegs;
		}

		if (!cps_counted_as_foldedto.count(cp_to))
		{
			cps_counted_as_foldedto[cp_to] = 1;
			++numofcps_to;
		}
	}

private:

	typedef std::map<long, char> flagset_type;

	flagset_type ucf_countedsegnos;	//  the set of segment nos marked as "counted" for case-folding.
	flagset_type rev_countedsegnos;	//  the set of segment nos marked as "counted" for reverse lookup.
	flagset_type cps_counted_as_foldedto;	//  the set of code points marked as "folded to".
};

bool read_file(std::string &str, const char *const filename)
{
	FILE *const fp = std::fopen(filename, "r");

	std::fprintf(stdout, "Reading '%s'... ", filename);

	if (fp)
	{
		static const std::size_t bufsize = 1024;
		char buffer[bufsize];

		for (;;)
		{
			const std::size_t size = std::fread(buffer, 1, bufsize, fp);

			if (!size)
				break;

			str.append(buffer, size);
		}
		std::fclose(fp);
		std::fputs("done.\n", stdout);
		return true;
	}
	std::fputs("error!\n", stdout);
	return false;
}


int main(const int argc, const char *const *const argv)
{
	const char *infilename = "CaseFolding.txt";
	const char *outfilename = "srell_ucfdata.hpp";
	const char *const indent = "\t\t\t";
	std::string buf;

	if (argc > 1)
	{
		infilename = argv[1];

		if (argc > 2)
			outfilename = argv[2];
	}

	if (read_file(buf, infilename))
	{
		FILE *const fp = std::fopen(outfilename, "w");

		if (fp)
		{
			static const srell::regex re_line("^.*$", srell::regex::multiline);
			const srell::cregex_iterator eos;
			srell::cregex_iterator iter(buf.c_str(), buf.c_str() + buf.size(), re_line);
			srell::cmatch match;
			ucf_data ucfdata;

			for (; iter != eos; ++iter)
			{
				if (iter->length(0))
				{
					static const srell::regex re_datainfo("^# (.*)$");

					if (!srell::regex_match((*iter)[0].first, (*iter)[0].second, match, re_datainfo))
					{
						std::fputs("\n", fp);
						break;
					}

					std::fprintf(fp, "//  %s\n", match.str(1).c_str());
				}
			}

			std::fputs("template <typename T1, typename T2, typename T3>\nstruct unicode_casefolding\n{\n\tstatic const T1 *table()\n\t{\n\t\tstatic const T1 ucftable[] =\n\t\t{\n", fp);

			for (; iter != eos; ++iter)
			{
				static const srell::regex re_cfdata("^\\s*([0-9A-Fa-f]+); ([CS]); ([0-9A-Fa-f]+);\\s*#\\s*(.*)$");
				const srell::cmatch &line = *iter;

				if (srell::regex_match(line[0].first, line[0].second, match, re_cfdata))
				{
					const std::string from(match[1]);
					const std::string to(match[3]);
					const std::string type(match[2]);
					const std::string name(match[4]);

					ucfdata.update(from, to);

					std::fprintf(fp, "%s{ 0x%s, 0x%s },\t//  %s; %s\n", indent, from.c_str(), to.c_str(), type.c_str(), name.c_str());
				}
				else
				{
					static const srell::regex re_comment_or_emptyline("^#.*|^$");

					if (!srell::regex_match(line[0].first, line[0].second, re_comment_or_emptyline))
						std::fprintf(fp, "%s//  %s\n", indent, line.str(0).c_str());
				}
			}
			std::fprintf(fp, "%s{ 0, 0 }\n\t\t};\n\t\treturn ucftable;\n\t}\n", indent);

			std::fprintf(fp, "\tstatic const T2 ucf_maxcodepoint = 0x%.6lX;\n", ucfdata.ucf_maxcodepoint);
			std::fprintf(fp, "\tstatic const T3 ucf_deltatablesize = 0x%X;\n", ucfdata.ucf_numofsegs << 8);

			std::fprintf(fp, "\tstatic const T2 rev_maxcodepoint = 0x%.6lX;\n", ucfdata.rev_maxcodepoint);
			std::fprintf(fp, "\tstatic const T3 rev_indextablesize = 0x%X;\n", ucfdata.rev_numofsegs << 8);
			std::fprintf(fp, "\tstatic const T3 rev_charsettablesize = %u;\t//  1 + %u * 2 + %u\n};\n", ucfdata.numofcps_to * 2 + ucfdata.numofcps_from + 1, ucfdata.numofcps_from, ucfdata.numofcps_to);
			std::fclose(fp);

			std::fprintf(stdout, "MaxDelta: %+ld (U+%.4lX->U+%.4lX)\n", ucfdata.maxdelta, ucfdata.maxdelta_cp, ucfdata.maxdelta_cp + ucfdata.maxdelta);
		}
		else
			std::fprintf(stdout, "Could not open '%s' ...", outfilename);
	}
	return 0;
}
