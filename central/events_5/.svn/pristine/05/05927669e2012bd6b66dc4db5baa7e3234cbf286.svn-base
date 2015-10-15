#ifdef OS_Linux
#pragma pack(1)
#endif

/// Definicion de la clase de eventos de mensajes offline

class TOCierre: public Event
{
protected:
   str_timeout_event	data;
   Iso_Msg    		*mensaje_iso;

public:
   TOCierre(int msg_pid, char *msg_data, int data_len);
   ~TOCierre();
   int EnviarMsg(int orig_pid, char *msg, int len);
   int ProcessIt();
};

