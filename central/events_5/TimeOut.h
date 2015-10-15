#ifdef OS_Linux
#pragma pack(1)
#endif

class TimeOut: public Event
{
protected:
   str_timeout_event	data;

public:
   TimeOut(int msg_pid, char *msg_data, int data_len);

   int ProcessIt();
};

