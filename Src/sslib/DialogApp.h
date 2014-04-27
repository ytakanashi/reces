//DialogApp.h

#ifndef _DIALOGAPP_H_E833C322_83F7_44df_A7BF_D46134FB5C22
#define _DIALOGAPP_H_E833C322_83F7_44df_A7BF_D46134FB5C22


extern int WINAPI WinMain(HINSTANCE instance_handle,HINSTANCE prev_instance_handle,LPSTR cmd_line,int cmd_show);

namespace sslib{
class DialogApp{
	friend DialogApp* app();
	friend int WINAPI ::WinMain(HINSTANCE instance_handle,HINSTANCE prev_instance_handle,LPSTR cmd_line,int cmd_show);

protected:
	DialogApp(){
		this_ptr=this;
	}

	virtual ~DialogApp(){
	}

private:
	static DialogApp* this_ptr;

protected:
	virtual bool init();
	virtual bool run(CommandArgument& cmd_arg,int cmd_show)=0;
	virtual void cleanup();
};

//namespace sslib
}

#endif //_DIALOGAPP_H_E833C322_83F7_44df_A7BF_D46134FB5C22
