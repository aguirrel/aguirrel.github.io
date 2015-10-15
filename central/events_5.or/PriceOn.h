#ifdef OS_Linux
#pragma pack(1)
#endif

typedef struct trxRtaTPV
{
	char szCodAut[ 6+1];
	char szComerc[15+1];
	char szTermin[ 8+1];
	char szNroLot[ 3+1];
	char szPlnIso[ 1+1];
	char szProtoc[ 1+1];
	char szRespCA[ 3+1];
	char szFecOri[ 6+1];
	char szTicOri[ 4+1];
	char szPlanSF[ 2+1];
} trxRtaTPV;

class PriceOnLine: public Event
{
protected:
   Iso_Msg	*mensaje_iso;
   Price_Msg	*mensaje_price;

public:
   PriceOnLine(int msg_pid, char *msg_data, int data_len);
   ~PriceOnLine();
   int Armar_y_Enviar_Respuesta(char*, struct trxRtaTPV*);

   int ProcessIt();
};

