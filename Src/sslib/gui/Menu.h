//Menu.h

#ifndef _MENU_H_8CD264B0_9B59_4f47_9E3B_34423D2EFE39
#define _MENU_H_8CD264B0_9B59_4f47_9E3B_34423D2EFE39

namespace sslib{
class Menu{
public:
	Menu(HINSTANCE inst=NULL):
		m_inst(inst),
		m_menu(NULL),
		m_sub_menu(NULL){}

	virtual ~Menu(){destory();}

private:
	HINSTANCE m_inst;
	HMENU m_menu;
	HMENU m_sub_menu;

public:
	bool load(UINT id,int sub_pos=0);
	bool destory();
	int popup(HWND handle,int x,int y,UINT flags=TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL|TPM_RETURNCMD);
	int popup(HWND handle,HWND control_handle,UINT flags=TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL|TPM_RETURNCMD);
	tstring string(UINT id,bool by_pos=false);

	HMENU handle()const{return m_menu;}
	HMENU sub_handle()const{return m_sub_menu;}
};

//namespace sslib
}

#endif //_MENU_H_8CD264B0_9B59_4f47_9E3B_34423D2EFE39
