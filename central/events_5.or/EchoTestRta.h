#ifdef OS_Linux
#pragma pack(1)
#endif

class EchoTestRta: public Event
{
protected:
   Iso_Msg	*mensaje_iso;

public:
	EchoTestRta(int msg_pid, char *msg_data, int data_len);
	~EchoTestRta();
	int ProcessIt();
	int EnviarMsg(int orig_pid, char *msg, int len);

};

