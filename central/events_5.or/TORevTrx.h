#ifdef OS_Linux
#pragma pack(1)
#endif

/// Definicion de la clase de evento de Time Out para el mensaje de envio de 
/// reverso.

class TORevTrx: public Event
{
protected:
   str_timeout_event	data;
   Iso_Msg    		*mensaje_iso;

public:
	TORevTrx(int msg_pid, char *msg_data, int data_len);
	~TORevTrx();
	int ProcessIt();
	int EnviarMsg(int orig_pid, char *msg, int len);
};

