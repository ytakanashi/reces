//Tab.cpp
//タブコントロールクラス


#include"../../StdAfx.h"
#include"../sslib.h"


namespace sslib{


//タブを追加
bool Tab::insert(const Dialog& dlg,const TCHAR* title,int index){
	TC_ITEM tab_item={0};

	tab_item.mask=TCIF_TEXT;
	tab_item.pszText=const_cast<LPTSTR>(title);
	if(TabCtrl_InsertItem(handle(),index,&tab_item)==-1){
		return false;
	}

	//タブの座標取得
	RECT rc;
	LPPOINT pt=(LPPOINT)&rc;

	::GetClientRect(handle(),&rc);
	TabCtrl_AdjustRect(handle(),false,&rc);
	//座標変換
	::MapWindowPoints(handle(),parent_handle(),pt,2);

	//タブの位置とサイズの調整
	::MoveWindow(dlg.handle(),
				rc.left,
				rc.top,
				rc.right-rc.left,
				rc.bottom-rc.top,
				false
	);
	return true;
}

//namespace sslib
}
