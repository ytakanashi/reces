//ListView.h

#ifndef _LISTVIEW_H_D2947840_B121_47cc_8B5C_977A97005545
#define _LISTVIEW_H_D2947840_B121_47cc_8B5C_977A97005545

namespace sslib{
class ListView:public Control{
public:
	ListView(HWND parent_handle,UINT resource_id,bool sub_class=false):
		Control(parent_handle,resource_id,sub_class){}
	virtual ~ListView(){}

public:
	inline int insertColumn(int index,const LVCOLUMN* column)const{
		return ListView_InsertColumn(handle(),index,column);
	}
	int insertColumn(int index,const TCHAR* text,int format=LVCFMT_LEFT,int width=-1,int sub_item=-1)const;

	inline int insertItem(const LVITEM* item)const{
		return ListView_InsertItem(handle(),item);
	}
	int insertItem(const TCHAR* text,int index=-1)const;

	inline bool deleteItem(int item)const{
		return ListView_DeleteItem(handle(),item)!=0;
	}

	inline bool getItem(LVITEM* item)const{
		return ListView_GetItem(handle(),item)!=0;
	}

	inline bool setItem(LVITEM* item)const{
		return ListView_SetItem(handle(),item)!=0;
	}

	LPARAM getItemData(int index=-1)const;

	bool setItemData(LPARAM data,int index=-1)const;

	inline int getNextItem(int start=-1,UINT flags=LVNI_ALL|LVNI_SELECTED)const{
		return ListView_GetNextItem(handle(),start,flags);
	}

	inline int getItemCount()const{
		return ListView_GetItemCount(handle());
	}

	tstring getItemText(int item=0,int sub_item=0)const;

	inline UINT getItemState(int item=0,UINT mask=LVIS_SELECTED)const{
		return ListView_GetItemState(handle(),item,mask);
	}

	inline void setItemState(int item=0,UINT state=LVIS_SELECTED,UINT mask=LVIS_SELECTED)const{
		ListView_SetItemState(handle(),item,state,mask);
	}

	inline void selectAll()const{
		setItemState(-1);
	}

	inline bool clear()const{
		return !!ListView_DeleteAllItems(handle());
	}

	inline DWORD getExtendedListViewStyle()const{
		return ListView_GetExtendedListViewStyle(handle());
	}
	inline void setExtendedListViewStyle(DWORD ex_style)const{
		ListView_SetExtendedListViewStyle(handle(),ex_style);
	}

	inline bool getCheckState(UINT index)const{
		return !!ListView_GetCheckState(handle(),index);
	}
	inline void setCheckState(UINT index,bool check=true)const{
		ListView_SetCheckState(handle(),index,check);
	}
};

//namespace sslib
}

#endif //_LISTVIEW_H_D2947840_B121_47cc_8B5C_977A97005545
