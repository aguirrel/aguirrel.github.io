#ifdef OS_Linux
#pragma pack(1)
#endif

class Batch: public Event
{
protected:
   Iso_Msg	*mensaje_iso;

public:
   Batch(int msg_pid, char *msg_data, int data_len);
   ~Batch();

   int ProcessIt();
};

