#ifdef OS_Linux
#pragma pack(1)
#endif

// Definicion de la clase de eventos de timeout en mensajes offline.

class TmtTrxOffLine: public Event
{
protected:
   str_timeout_event	tmtInfo;         // Informacion del timeout.
   long                 tmtPid;          // Pid del timeout.

public:
	TmtTrxOffLine(int msg_pid, char *msg_data, int data_len);
	~TmtTrxOffLine();

	int ProcessIt();
	int EnviarMsg(int orig_pid, char *msg, int len);
};

