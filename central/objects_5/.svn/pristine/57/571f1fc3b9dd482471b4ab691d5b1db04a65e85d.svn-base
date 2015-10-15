#ifndef LINKX25_H
#define LINKX25_H

#include "LinkCVC.h"

class X25 : public CVC
{
protected:
    static int initDone;

#ifdef OS_AIX
    struct cb_link_name_struct    x25LinkDefs;			// X25 Link Info.
    struct ctr_array_struct       x25CtrArray[1];		// X25 Counters.
    struct cb_call_struct         x25CallDefs;			// X25 Call Info.
    struct cb_msg_struct          x25RcvMsg;			// X25 In Msg Struct
    struct cb_clear_struct        x25ClearDefs;			// X25 Hang up info.
    struct cb_data_struct         x25Data;				// X25 Send Msg info.
#endif
	int		connId;						                // Connection ID.
    int		ctrId;							            // Counter ID.
    char	linkName[40];								// Nombre del canal.
    char    CalledAddress[40];							// X25 Addrs To call.
    int	    x25LastError;
    int		sysLastError;
	unsigned long codUserData[2];

public:
	X25() {};
	X25(char *linkName);
	int  Open(char*, char*, char*);
	int  Close();
	int  Write(char *data, int len);
	int  Read(char *data, int *len, int *tipo);
	int  MsgPnd();
	int  setCodUserData(char data[2][11]);
	int  GetErrno()					{return x25LastError;}
	int  GetErSys()					{return sysLastError;}
};

#endif