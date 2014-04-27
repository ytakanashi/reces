//ApiHook

#ifndef _APIHOOK_H_557863B7_F0A3_400b_AE0E_9C6E02591C97
#define _APIHOOK_H_557863B7_F0A3_400b_AE0E_9C6E02591C97



namespace sslib{
namespace apihook{

void replace(PCSTR callee_mod_name,PROC current_pfn,PROC new_pfn,HMODULE mod_caller);

//namespace apihook
}
//namespace sslib
}

#endif //_APIHOOK_H_557863B7_F0A3_400b_AE0E_9C6E02591C97
