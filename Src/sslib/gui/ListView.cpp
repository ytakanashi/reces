//ListView.cpp
//リストビューコントロールクラス


#include"../../StdAfx.h"
#include"../sslib.h"


namespace sslib{

int ListView::insertColumn(int index,const TCHAR* text,int format,int width,int sub_item)const{
	LVCOLUMN column={0};

	column.mask=LVCF_FMT|LVCF_TEXT;
	column.fmt=format;
	column.pszText=(LPTSTR)text;
	if(width!=-1){
		column.mask|=LVCF_WIDTH;
		column.cx=width;
	}
	if(sub_item!=-1){
		column.mask|=LVCF_SUBITEM;
		column.iSubItem=sub_item;
	}
	return insertColumn(index,&column);
}

int ListView::insertItem(const TCHAR* text,int index)const{
	LVITEM item={0};

	item.mask=LVIF_TEXT;
	item.pszText=(LPTSTR)text;
	item.iItem=(index!=-1)?index:getItemCount();
	item.iSubItem=0;
	return insertItem(&item);
}

LPARAM ListView::getItemData(int index)const{
	LVITEM item={0};

	item.mask=LVIF_PARAM;
	item.iItem=(index!=-1)?index:getItemCount();

	return (getItem(&item))?item.lParam:0;

}

bool ListView::setItemData(LPARAM data,int index)const{
	LVITEM item={0};

	item.mask=LVIF_PARAM;
	item.iItem=(index!=-1)?index:getItemCount();
	item.iSubItem=0;
	item.lParam=data;
	return setItem(&item);
}

tstring ListView::getItemText(int item,int sub_item)const{
	std::vector<TCHAR> text(2048);

	ListView_GetItemText(handle(),item,sub_item,&text[0],text.size());
	return &text[0];
}


//namespace sslib
}
