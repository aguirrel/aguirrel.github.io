#ifdef OS_Linux
#pragma pack(1)
#endif

class EchoTest: public Event
{
protected:
   str_ping_data	ping_data;
   Iso_Msg		*mensaje_iso;

public:
	EchoTest(int msg_pid, char *msg_data, int data_len);
	~EchoTest();
	int ProcessIt();
	int EnviarMsg(int orig_pid, char *msg, int len);
};

