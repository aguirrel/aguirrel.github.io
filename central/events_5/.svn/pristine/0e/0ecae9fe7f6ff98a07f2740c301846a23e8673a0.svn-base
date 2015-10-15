#ifdef OS_Linux
#pragma pack(1)
#endif

// Estructura de informacion del hea de transacciones offline.


// Definicion de la clase de eventos de mensajes offline

class RtaTrxOffLine: public Event
{
protected:
   char     CodTarjeta  [5];    // Codigo de tarjeta que tiene offlines.
   Iso_Msg  * IsoMsg;           // Mensaje a enviar. 

public:
	RtaTrxOffLine(int msg_pid, char *msg_data, int data_len);
	~RtaTrxOffLine();
	int ProcessIt();
	int EnviarMsg(int orig_pid, char *msg, int len);
};

