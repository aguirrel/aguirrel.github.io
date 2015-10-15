#include "ConexSocket.h"

int main()
{
	ConexSocket ComCnt;

	ComCnt.OpenClient("127.0.0.1", 65240);
	ComCnt.SendMsg("C20023", 6);
	ComCnt.SendMsg("MENSAJE DE PRUEBA", 17);
	ComCnt.CloseServer();

	return 0;
}
