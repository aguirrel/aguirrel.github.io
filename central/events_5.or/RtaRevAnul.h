#ifdef OS_Linux
#pragma pack(1)
#endif

typedef struct
{
   long		origPid;
   char		rowid[50];
   char		msg_price[200];
   char		nro_caj_ca[9];
   char		nro_ca[3];
   char		nro_trc[7];
   int		msg_len;
} trxAnulHeap;

// Definicion de la clase de eventos de mensajes de anulacion

class RtaRevAnul: public Event
{
protected:
   char     CodTarjeta  [5];    // Codigo de tarjeta que tiene offlines.
   Iso_Msg  * IsoMsg;           // Mensaje a enviar. 
   char		szMsjeISO[512];
   int		iLMsjeISO;

public:
	RtaRevAnul(int msg_pid, char *msg_data, int data_len);
	~RtaRevAnul();
	int ProcessIt();
	int EnviarMsg(int orig_pid, char *msg, int len);

};

