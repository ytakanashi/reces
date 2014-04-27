//Misc.cpp
//その他諸々

#include"../StdAfx.h"
#include"sslib.h"


namespace sslib{
namespace misc{

CtrlCEvent* CtrlCEvent::m_this_ptr=NULL;

BOOL WINAPI CtrlCEvent::handlerRoutine(DWORD ctrl_type){
	CtrlCEvent* this_ptr=CtrlCEvent::m_this_ptr;

	for(ctrlc_event_list::iterator ite=this_ptr->m_ctrlc_event_list.begin(),
		end=this_ptr->m_ctrlc_event_list.end();
		ite!=end;
		++ite){
		(*ite)->ctrlCEvent();
	}
	return TRUE;
}

void CtrlCEvent::addCtrlCEvent(CtrlCEvent* event){
	if(m_ctrlc_event_list.empty()){
		::SetConsoleCtrlHandler(static_cast<PHANDLER_ROUTINE>(handlerRoutine),true);
	}
	m_ctrlc_event_list.push_back(event);
}

//namespace misc
}
//namespace sslib
}
