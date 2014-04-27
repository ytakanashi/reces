//Tab.h

#ifndef _TAB_H_2BDE4997_A3C7_49b3_A634_272A98488B1F
#define _TAB_H_2BDE4997_A3C7_49b3_A634_272A98488B1F

namespace sslib{
class Tab:public Control{
public:
	Tab(HWND parent_handle,UINT resource_id,bool sub_class=false):
		Control(parent_handle,resource_id,sub_class){}
	virtual ~Tab(){}
public:
	bool insert(const Dialog& dlg,const TCHAR* title,int index);
	inline void select(int index=0){
		::SendDlgItemMessage(parent_handle(),resource_id(),TCM_SETCURSEL,index,0);

		NMHDR hdr={};

		hdr.idFrom=resource_id();
		hdr.code=TCN_SELCHANGE;
		::SendMessage(parent_handle(),WM_NOTIFY,0,reinterpret_cast<LPARAM>(&hdr));
	}
};

//namespace sslib
}

#endif //_TAB_H_2BDE4997_A3C7_49b3_A634_272A98488B1F
