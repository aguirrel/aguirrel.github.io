#ifdef OS_Linux
#pragma pack(1)
#endif

// Estructura de informacion del heap de transacciones offline.

typedef struct
{
   long		orig_pid;
   char		cod_tar [ 3];
   char		nroSuc  [ 4];
   char		nroTerm [ 6];
   char		nroTrans[ 5];
   char		fechaTrx[11];
   char		NroCajCa[ 9];
   char		Rowid[30];
} trxRevHeap;


/// Definicion de la clase de eventos de mensajes offline

class RevTrx: public Event
{
protected:
   Iso_Msg	*IsoMsg;
   char		CodTarjeta[3];    // Codigo de tarjeta que tiene offlines.
   char		NroCajCa[9];	  // Caja con offlines
   char		NroTrx[5];

   int getIsoToSend(void); 

public:
	RevTrx(int msg_pid, char *msg_data, int data_len);
	~RevTrx();

	int ProcessIt();
	int EnviarMsg(int orig_pid, char *msg, int len);
};

