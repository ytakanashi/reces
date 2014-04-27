//DropFiles.h

#ifndef _DROPFILES_H_E6C3662E_0BA7_48cc_BECB_2C146DC8F6F8
#define _DROPFILES_H_E6C3662E_0BA7_48cc_BECB_2C146DC8F6F8



namespace sslib{
class DropFiles{
public:
	DropFiles(HDROP drop_handle):
		m_handle(drop_handle){};
	virtual ~DropFiles(){finish();}

private:
	HDROP m_handle;

public:
	inline void finish(){::DragFinish(handle());m_handle=NULL;}
	//ドロップされたファイルの総数を取得
	inline UINT getCount()const{return ::DragQueryFile(handle(),0xffffffff,NULL,0);}

	//ファイルパスを取得
	tstring getFile(int index=0);

	//マウスカーソルの座標を取得
	inline bool getPoint(POINT* pt)const{return DragQueryPoint(handle(),pt)!=0;}

	inline HDROP handle()const{return m_handle;}
};

//namespace sslib
}

#endif //_DROPFILES_H_E6C3662E_0BA7_48cc_BECB_2C146DC8F6F8
