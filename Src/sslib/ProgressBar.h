//ProgressBar.h

#ifndef _PROGRESSBAR_H_88F92C02_11A9_4f83_B8D6_8910F8A5D0F5
#define _PROGRESSBAR_H_88F92C02_11A9_4f83_B8D6_8910F8A5D0F5


namespace sslib{
class ProgressBar{
public:
	enum PROGRESSBAR_RESET_OPT{
		RESET_LAST_FRACTION_DONE=1<<0,
		RESET_LAST_DOTS=1<<1,
		RESET_LAST_MSG_WIDTH=1<<2,
	};

public:
	ProgressBar(Console& stdOut);
	virtual ~ProgressBar();

private:
	Console& m_stdout;

	static const int m_progress_length=70;
	static TCHAR m_progress[m_progress_length+1];
	static TCHAR m_background[m_progress_length+1];

	COORD m_begin_pos;
	bool m_first_time;
	//']'のx座標
	int m_progress_right_end_pos_x;
	//最後に表示した進捗率
	unsigned int m_last_fraction_done;
	//最後に表示したプログレスバーの目盛数
	int m_last_dots;
	//最後に表示した文字列の幅
	int m_last_msg_width;

public:
	//値を更新
	bool update(long long done,long long total,const TCHAR* msg=NULL);
	//各値をリセット
	inline void reset(int opt,int value=0){
		if(opt&RESET_LAST_FRACTION_DONE){
			m_last_fraction_done=value;
		}
		if(opt&RESET_LAST_DOTS){
			m_last_dots=value;
		}
		if(opt&RESET_LAST_MSG_WIDTH){
			m_last_msg_width=value;
		}
	}
};

//namespace sslib
}

#endif //_PROGRESSBAR_H_88F92C02_11A9_4f83_B8D6_8910F8A5D0F5
