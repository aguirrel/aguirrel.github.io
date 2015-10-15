#ifdef OS_Linux
#pragma pack(1)
#endif

class PriceOnLineRta: public Event
{
protected:
   Iso_Msg	*mensaje_iso;
   Price_Msg	*mensaje_price;

public:
   PriceOnLineRta(int msg_pid, char *msg_data, int data_len);
   ~PriceOnLineRta();

   int ProcessIt();
};

