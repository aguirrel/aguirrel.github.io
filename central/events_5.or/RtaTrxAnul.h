#ifdef OS_Linux
#pragma pack(1)
#endif

// Definicion de la clase de eventos de mensajes de anulacion

class RtaTrxAnul: public Event
{
protected:
   char     CodTarjeta  [5];    // Codigo de tarjeta que tiene offlines.
   Iso_Msg  * IsoMsg;           // Mensaje a enviar. 
   char		szMsjeISO[512];
   int		iLMsjeISO;

public:
	RtaTrxAnul(int msg_pid, char *msg_data, int data_len);
	~RtaTrxAnul();

	int ProcessIt();
	int EnviarMsg(int orig_pid, char *msg, int len);
};

