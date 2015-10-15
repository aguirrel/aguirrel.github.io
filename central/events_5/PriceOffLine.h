#ifdef OS_Linux
#pragma pack(1)
#endif

/// Definicion de la clase de eventos de mensajes offline

class PriceOffLine: public Event
{
protected:
   Price_Msg  *mensaje_price;
   Iso_Msg    *mensaje_iso;

public:
   PriceOffLine(int msg_pid, char *msg_data, int data_len);
   ~PriceOffLine();

   int ProcessIt();
};

