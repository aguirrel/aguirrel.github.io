#ifdef OS_Linux
#pragma pack(1)
#endif

class ShutDown: public Event
{
protected:

public:
   ShutDown(int msg_pid, char *msg_data, int data_len);

   int ProcessIt();
};

