//ProgressBar.cpp
//プログレスバー

#include"../StdAfx.h"
#include"sslib.h"


namespace sslib{


TCHAR ProgressBar::m_progress[ProgressBar::m_progress_length+1];
TCHAR ProgressBar::m_background[ProgressBar::m_progress_length+1];


ProgressBar::ProgressBar(Console& stdOut):
	m_stdout(stdOut),
	m_begin_pos(),
	m_first_time(true),
	m_progress_right_end_pos_x(0),
	m_last_fraction_done(0),
	m_last_dots(0),
	m_last_msg_width(0){
		m_stdout.showCursor(false);

		for(int i=0;i<m_progress_length;i++)m_progress[i]=_T('=');
		m_progress[m_progress_length]='\0';
		m_progress[m_progress_length-1]='>';
		for(int i=0;i<m_progress_length;i++)m_background[i]=_T(' ');
		m_background[m_progress_length]='\0';
}

ProgressBar::~ProgressBar(){
	m_stdout.outputString(_T("\n"));
	m_stdout.showCursor(true);
}

//値を更新
bool ProgressBar::update(long long done,long long total,const TCHAR* msg){
	if(!total||done>total)return false;

	unsigned int fraction_done=std::min<unsigned int>((unsigned int)(done*100/total),100);
	int dots=std::min<int>(int((float(fraction_done)/100)*m_progress_length),m_progress_length);

	if(m_last_fraction_done==fraction_done&&
	   fraction_done==100)return false;

	m_last_fraction_done=fraction_done;

	if(m_first_time){
		m_stdout.getPosition(&m_begin_pos);
		m_begin_pos.X=0;
	}else{
		m_stdout.setPosition(m_begin_pos);
	}

	if(!m_first_time&&m_last_dots==dots){
		//更新する必要が無いので次行へ
		COORD new_pos=m_begin_pos;

		new_pos.Y++;
		m_stdout.setPosition(new_pos);
	}else{
		//プログレスバー表示
		int per_pos_x=m_stdout.outputString(_T("%3d%% ["),fraction_done);
		if(m_first_time){
			//']'のy座標を保存
			m_progress_right_end_pos_x=per_pos_x+m_progress_length;
		}


		if(m_last_dots==0){
			m_stdout.outputString(_T("%s"),
								  &m_progress[m_progress_length-dots]);
		}else if(m_last_dots<dots){
			//一文字前から出力
			m_stdout.setPosition(per_pos_x+m_last_dots-1,m_begin_pos.Y);
			m_stdout.outputString(_T("%s"),
								  &m_progress[m_progress_length-(dots-m_last_dots+1)]);
		}else{
			m_stdout.setPosition(per_pos_x+dots,m_begin_pos.Y);
			m_stdout.outputString(_T("%s%s"),
								  &m_progress[m_progress_length-1],
								  &m_background[m_progress_length-(m_last_dots-dots)]);
		}
		m_stdout.setPosition(m_progress_right_end_pos_x,m_begin_pos.Y);
		m_stdout.outputString(_T("]\n"));
	}

	m_last_dots=dots;

	if(msg!=NULL){
		int msg_width=0;

		//メッセージ表示
		tstring msg_str=format(_T("   => \'%s\'"),msg);

		msg_width=::WideCharToMultiByte(CP_ACP,
										0,
										msg_str.c_str(),
										msg_str.length(),
										NULL,
										0,
										"　",
										NULL);
		m_stdout.outputString(msg_str.c_str());

		//"   =>"がある行を探し、移動
		static const TCHAR header[]=_T("   =>");
		static const int buffer_size=ARRAY_SIZEOF(header);
		static TCHAR buffer[buffer_size];

		m_stdout.getPosition(&m_begin_pos);
		m_begin_pos.X=0;

		DWORD read_chars=0;

		for(;m_begin_pos.Y;m_begin_pos.Y--){
			::ReadConsoleOutputCharacter(m_stdout.handle(),
										 buffer,
										 ARRAY_SIZEOF(header),
										 m_begin_pos,
										 &read_chars);
			buffer[read_chars-1]='\0';
			if(lstrcmp(header,buffer)==0){
				break;
			}
		}

		//進捗率表示行へ
		m_begin_pos.Y--;

		if(m_last_msg_width>msg_width){
			m_stdout.clear(m_last_msg_width-msg_width+1);
		}
		m_last_msg_width=msg_width;
	}

	if(m_first_time){
		m_first_time=false;
	}

	return true;
}

//namespace sslib
}
