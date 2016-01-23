//Misc.cpp
//その他諸々

#include"../StdAfx.h"
#include"sslib.h"


namespace sslib{
namespace misc{

CtrlCEventManager* CtrlCEventManager::m_this_ptr=NULL;

BOOL WINAPI CtrlCEventManager::handlerRoutine(DWORD ctrl_type){
	CtrlCEventManager* this_ptr=CtrlCEventManager::m_this_ptr;

	for(ctrlc_event_list::iterator ite=this_ptr->m_ctrlc_event_list.begin(),
		end=this_ptr->m_ctrlc_event_list.end();
		ite!=end;
		++ite){
		(*ite)->ctrlCEvent();
	}
	return TRUE;
}

void CtrlCEventManager::addCtrlCEvent(CtrlCEvent* event){
	if(m_ctrlc_event_list.empty()){
		::SetConsoleCtrlHandler(static_cast<PHANDLER_ROUTINE>(CtrlCEventManager::handlerRoutine),true);
	}
	m_ctrlc_event_list.push_back(event);
}

//汎用性に乏しいスレッド関係関数たち
namespace thread{
	HANDLE create(unsigned (__stdcall*func)(void*),unsigned*addr,void* this_ptr,void* data){
		Event ready;
		PARAM thread_param(this_ptr,ready,data);

		HANDLE handle=(HANDLE)_beginthreadex(NULL,0,func,reinterpret_cast<void*>(&thread_param),0,addr);

		//スレッド側はready.signal()で応えること
		ready.wait();
		return handle;
	}

	bool post(DWORD id,UINT msg,void* data1,void* data2,void* this_ptr){
		Event ready;
		PARAM thread_param(this_ptr,ready,data1);

		bool result=::PostThreadMessage(id,msg,(WPARAM)&thread_param,(LPARAM)data2)!=0;

		//スレッド側はready.signal()で応えること
		ready.wait();
		return result;
	}

	bool close(INFO& info){
		DWORD exit_code=0;

		::GetExitCodeThread(info.handle,&exit_code);
		if(exit_code==STILL_ACTIVE){
			::PostThreadMessage(info.id,WM_QUIT,0,0);
			::WaitForSingleObject(info.handle,INFINITE);
			SAFE_CLOSE(info.handle);
			return true;
		}
		return true;
	}
}//namespace thread

//std::listの重複削除(順序保持)
bool undupList(std::list<tstring>* list){
	typedef std::pair<int,tstring> list_info;

	INNER_FUNC(sortNumber,
		bool operator()(list_info& list1,list_info& list2){
			return list1.first<list2.first;
		}
	);
	INNER_FUNC(sortString,
		bool operator()(list_info& list1,list_info& list2){
			return list1.second<list2.second;
		}
	);
	INNER_FUNC(sameString,
		bool operator()(list_info& list1,list_info& list2){
			return list1.second==list2.second;
		}
	);

	if(!list)return false;

	//順序保持用リスト
	std::list<list_info> list_bak;

	for(std::list<tstring>::const_iterator ite=list->begin(),
		end=list->end();
		ite!=end;++ite){
		list_bak.push_back(list_info(list_bak.size(),*ite));
	}
	list_bak.sort(sortString);
	list_bak.unique(sameString);
	list_bak.sort(sortNumber);

	list->clear();
	for(std::list<list_info>::const_iterator ite=list_bak.begin(),
		end=list_bak.end();
		ite!=end;++ite){
		list->push_back(ite->second);
	}
	return true;
}

//namespace misc
}
//namespace sslib
}
