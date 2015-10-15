#ifdef OS_Linux
#pragma pack(1)
#endif

class PriceRev: public Event
{
protected:
   Iso_Msg	*mensaje_iso;
   Price_Msg	*mensaje_price;
   Price_Msg	*mensaje_price_rta;

public:
   PriceRev(int msg_pid, char *msg_data, int data_len);
   ~PriceRev();

   int ProcessIt();
   inline char* GetName() { return "PriceRev"; }
};

