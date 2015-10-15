#ifdef OS_Linux
#pragma pack(1)
#endif

class Cierre: public Event
{
protected:
   Iso_Msg	*mensaje_iso;

public:
   Cierre(int msg_pid, char *msg_data, int data_len);
   ~Cierre();

   int ProcessIt();
};

