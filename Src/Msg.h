//Msg.h

//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`
//              reces Ver.0.00r27 by x@rgs
//              under NYSL Version 0.9982
//
//`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`~^`


#ifndef _MSG_H_03322F3D_C92C_4c3c_859E_74C387176F78
#define _MSG_H_03322F3D_C92C_4c3c_859E_74C387176F78


namespace msg{

bool info(const TCHAR* msg,...);
void err(const TCHAR* msg,...);
void lasterr();

//namespace msg
}

#endif //_MSG_H_03322F3D_C92C_4c3c_859E_74C387176F78
