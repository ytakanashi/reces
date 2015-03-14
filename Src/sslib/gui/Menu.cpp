//Menu.cpp
//手抜きメニュー


#include"../../StdAfx.h"
#include"../sslib.h"


namespace sslib{


bool Menu::load(UINT id,int sub_pos){
	m_menu=::LoadMenu(m_inst,MAKEINTRESOURCE(id));
	m_sub_menu=::GetSubMenu(m_menu,sub_pos);
	return m_menu&&m_sub_menu;
}

bool Menu::destory(){
	return !!::DestroyMenu(m_menu);
}

int Menu::popup(HWND handle,int x,int y,UINT flags){
	return ::TrackPopupMenu(m_sub_menu,flags,x,y,0,handle,NULL);
}

int Menu::popup(HWND handle,HWND control_handle,UINT flags){
	RECT rc={0};

	::GetWindowRect(control_handle,&rc);
	return popup(handle,rc.left,rc.bottom,flags);
}

tstring Menu::string(UINT id,bool by_pos){
	std::vector<TCHAR> buffer(512);
	MENUITEMINFO menu_info={sizeof(MENUITEMINFO)};

	menu_info.fMask=MIIM_STRING;
	menu_info.dwTypeData=&buffer[0];
	menu_info.cch=buffer.size();
	::GetMenuItemInfo(m_sub_menu,id,by_pos,&menu_info);
	return &buffer[0];
}


//namespace sslib
}
