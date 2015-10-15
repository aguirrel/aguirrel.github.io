#ifdef OS_Linux
#pragma pack(1)
#endif

class CierreRta: public Event
{
protected:
   Iso_Msg	*mensaje_iso;

public:
   CierreRta(int msg_pid, char *msg_data, int data_len);
   ~CierreRta();
   int EnviarMsg(int orig_pid, char *msg, int len);
   int ProcessIt();
};

