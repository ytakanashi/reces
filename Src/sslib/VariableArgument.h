//VariableArgument.h

#ifndef _VARIABLEARGUMENT_H_BD113B6C_71C6_4691_8A2E_7BAA24851A35
#define _VARIABLEARGUMENT_H_BD113B6C_71C6_4691_8A2E_7BAA24851A35



namespace sslib{
class VariableArgument:private misc::NonCopyable<VariableArgument>{
public:
	VariableArgument(const TCHAR* format,const va_list argp);
	VariableArgument(const TCHAR* format,...);
	void add(const TCHAR* format,...);
	~VariableArgument();
	inline const TCHAR* get()const{return &m_buffer[0];}
	inline const TCHAR* get(int i)const{return &m_buffer[i];}
	inline const std::vector<TCHAR>& getVector()const{return m_buffer;}

private:
	std::vector<TCHAR> m_buffer;
};

//namespace sslib
}

#endif //_VARIABLEARGUMENT_H_BD113B6C_71C6_4691_8A2E_7BAA24851A35
