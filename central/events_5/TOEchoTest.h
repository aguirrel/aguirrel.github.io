#ifdef OS_Linux
#pragma pack(1)
#endif

/// Definicion de la clase de evento de Time Out para el mensaje de EchoTest

class TOEchoTest: public Event
{
protected:
   str_timeout_event	data;
   Iso_Msg    		*mensaje_iso;

public:
	TOEchoTest(int msg_pid, char *msg_data, int data_len);
	~TOEchoTest();
	int ProcessIt();
	int EnviarMsg(int orig_pid, char *msg, int len);
};

