#ifdef OS_Linux
#pragma pack(1)
#endif

// Definicion de la clase de eventos de mensajes offline

class RevTrxRta: public Event
{
protected:
   char     CodTarjeta[5];    // Codigo de tarjeta que tiene offlines.
   Iso_Msg  *IsoMsg;          // Mensaje a enviar. 
   int      orig_pid; 

public:
	RevTrxRta(int msg_pid, char *msg_data, int data_len);
	~RevTrxRta();
	int ProcessIt();
	int EnviarMsg(int orig_pid, char *msg, int len);
};

