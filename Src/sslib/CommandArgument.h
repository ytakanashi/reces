//CommandArgument.h

#ifndef _COMMANDARGUMENT_H_63271DE6_2B65_493a_B3DD_C81CCAEB4AA5
#define _COMMANDARGUMENT_H_63271DE6_2B65_493a_B3DD_C81CCAEB4AA5



namespace sslib{
class CommandArgument{
public:
	enum OPTIONS{
		READ_STDIN=1<<0,
		DO_WILDCARD=1<<1
	};

public:
	CommandArgument(DWORD opt=DO_WILDCARD);
	~CommandArgument(){}

private:
	std::vector<tstring> m_args;
	std::vector<tstring> m_stdinput;
	std::vector<tstring> m_options;
	std::vector<tstring> m_filepaths;

public:
	inline std::vector<tstring>& args(){return m_args;}
	inline std::vector<tstring>& stdinput(){return m_stdinput;}
	inline std::vector<tstring>& options(){return m_options;}
	inline std::vector<tstring>& filepaths(){return m_filepaths;}
};

//namespace sslib
}

#endif //_COMMANDARGUMENT_H_63271DE6_2B65_493a_B3DD_C81CCAEB4AA5
