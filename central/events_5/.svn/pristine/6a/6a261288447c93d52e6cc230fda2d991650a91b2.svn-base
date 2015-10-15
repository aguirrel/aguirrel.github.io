#ifdef OS_Linux
#pragma pack(1)
#endif

class BatchRta: public Event
{
protected:
   Iso_Msg	*mensaje_iso;

public:
   BatchRta(int msg_pid, char *msg_data, int data_len);
   ~BatchRta();
   int EnviarMsg(int orig_pid, char *msg, int len);
   int ProcessIt();
};

