//DialogApp.h

#ifndef _DIALOGAPP_H_E833C322_83F7_44df_A7BF_D46134FB5C22
#define _DIALOGAPP_H_E833C322_83F7_44df_A7BF_D46134FB5C22


extern int WINAPI WinMain(HINSTANCE instance_handle,HINSTANCE prev_instance_handle,LPSTR cmd_line,int cmd_show);

namespace sslib{
class DialogApp{
	friend int WINAPI ::WinMain(HINSTANCE instance_handle,HINSTANCE prev_instance_handle,LPSTR cmd_line,int cmd_show);

protected:
	DialogApp():
		m_instance_handle(NULL){
	}

	virtual ~DialogApp(){
	}

protected:
	HINSTANCE m_instance_handle;

protected:
	virtual bool init();
	virtual bool run(CommandArgument& cmd_arg,int cmd_show)=0;
	virtual void cleanup();

public:
	HINSTANCE inst()const{return m_instance_handle;}
};

//namespace sslib
}

extern sslib::DialogApp* app();

#endif //_DIALOGAPP_H_E833C322_83F7_44df_A7BF_D46134FB5C22
