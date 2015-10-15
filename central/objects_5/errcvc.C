#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TAM   64
#define TAM_LIN	  75
#define MAX_TCP  100
#define MAX_ISO  111
#define LON_ISO   30

typedef struct ST_X25 
{
	char sErr[3+1];
	char sEti[16+1];
	char sDet[512+1];
} ST_X25;

ST_X25 stX25[MAX_TAM] =
{
	{ "200" , "X25AUTH"        , "The calling application does not have system permission to control the status of the link."                                                                                                                                                        },
	{ "201" , "X25BADID"       , "The connection identifier or listen identifier is invalid."                                                                                                                                                                                        },
	{ "202" , "X25CALLED"      , "The called address is invalid. Check that the address is correct and is a null-terminated string."                                                                                                                                                 },
	{ "203" , "X25CALLING"     , "The calling address is invalid. Check that the address is correct and is a null-terminated string."                                                                                                                                                },
	{ "205" , "X25CTRUSE"      , "The counter has a nonzero value."                                                                                                                                                                                                                  },
	{ "206" , "X25INIT"        , "X.25 is already initialized for this X.25 port, so it cannot be initialized again."                                                                                                                                                                },
	{ "207" , "X25INVFAC"      , "An optional facility requested is invalid. Check the structure cb_fac_struct."                                                                                                                                                                     }, 
	{ "208" , "X25INVMON"      , "The monitoring mode is invalid."                                                                                                                                                                                                                   },
	{ "209" , "X25LINKUSE"     , "The X.25 port still has virtual circuits established; it may still be in use. Either free all virtual circuits or disconnect the port using the override."                                                                                         },
	{ "210" , "X25LONG"        , "The parameter is too long. Check each of the parameters for this subroutine."                                                                                                                                                                      },
	{ "211" , "X25NAMEUSED"    , "Calls for this name are already being listened for."                                                                                                                                                                                               },
	{ "213" , "X25NOCARD"      , "The X.25 adapter is either not installed or not functioning."                                                                                                                                                                                      },
	{ "214" , "X25NOCTRS"      , "No counters are available."                                                                                                                                                                                                                        },
	{ "215" , "X25NODATA"      , "No data has arrived for this connection identifier. Issue the x25_ctr_wait subroutine to be notified when data arrives."                                                                                                                           },
	{ "216" , "X25NODEVICE"    , "The X.25 device driver is either not installed or not functioning."                                                                                                                                                                                },
	{ "218" , "X25NOLINK"      , "The X.25 port is not connected. Issue the x25_link_connect subroutine, or use the xmanage command to connect it."                                                                                                                                  },
	{ "219" , "X25NONAME"      , "The name is not in the routing list. Add the name or use one that is already in the list."                                                                                                                                                         },
	{ "223" , "X25PROTOCOL"    , "An X.25 protocol error occurred."                                                                                                                                                                                                                  },
	{ "224" , "X25PVCUSED"     , "This permanent virtual circuit (PVC) is already allocated to another application. The other application must free the PVC before it can be used."                                                                                                  },
	{ "226" , "X25RESETCLEAR"  , "The call was reset or cleared during processing. Issue the x25_receive subroutine to obtain the reset-indication or clear-indication packet. Then issue the x25_reset_confirm or x25_clear_confirm subroutine, as necessary."                      },
	{ "228" , "X25TABLE"       , "The routing list cannot be updated because the xroute command is using it. Try again after xroute command has completed."                                                                                                                          },
	{ "229" , "X25TIMEOUT"     , "A time-out problem occurred."                                                                                                                                                                                                                      },
	{ "232" , "X25TOOMANYVCS"  , "No virtual circuits are free on the listed X.25 ports."                                                                                                                                                                                            },
	{ "251" , "X25AUTHCTR"     , "The application does not have permission to remove this counter because it did not issue the corresponding the x25_ctr_get subroutine."                                                                                                            },
	{ "252" , "X25AUTHLISTEN"  , "The application cannot listen to this name, because the corresponding entry in the routing list has a user name that excludes the user running the application. Use another routing list name, or change the user name in the routing list entry." },
	{ "253" , "X25BADCONNID"   , "The connection identifier is invalid."                                                                                                                                                                                                             },
	{ "254" , "X25BADDEVICE"   , "The X.25 port name is invalid."                                                                                                                                                                                                                    },
	{ "255" , "X25BADLISTENID" , "The listen identifier is invalid."                                                                                                                                                                                                                 },
	{ "256" , "X25INVCTR"      , "The specified counter does not exist. (In the case of the x25_ctr_wait subroutine, the counter is one of an array of counters.)"                                                                                                                   },
	{ "257" , "X25LINKUP"      , "The X.25 port is already connected."                                                                                                                                                                                                               },
	{ "259" , "X25MAXDEVICE"   , "Attempts have been made to connect more X.25 ports than are available. Check the smit configuration to see how many ports are available."                                                                                                          },
	{ "260" , "X25MONITOR"     , "X.25 traffic on this X.25 port is already being monitored by another application. The other application must stop monitoring before any other application can start monitoring."                                                                   },
	{ "261" , "X25NOACKREQ"    , "No packets currently require acknowledgment."                                                                                                                                                                                                      },
	{ "262" , "X25NOSUCHLINK"  , "The X.25 port does not exist. Check the smit configuration."                                                                                                                                                                                       },
	{ "263" , "X25NOTINIT"     , "The application has not initialized X.25 communications. Issue the x25_init subroutine. X25NOTPVC This is not defined as a permanent virtual circuit (PVC). Check the smit configuration."                                                         },
	{ "265" , "X25TRUNCTX"     , "The packet size is too big for internal buffers, so data cannot be sent."                                                                                                                                                                          },
	{ "267" , "X25SYSERR"      , "An error occurred that was not an X.25 error. Check the value of errno."                                                                                                                                                                           },
	{ 0     ,  0               ,  0 }
};

char stErr[MAX_ISO][LON_ISO]= 
{
	"APROBADA", 						/* 000 */
	"PEDIR AUTORIZACION",				/* 001 */
	"PEDIR AUTORIZACION",				/* 002 */
	"COMERCIO INVALIDO",       		    /* 003 */
	"CAPTURAR TARJETA",			        /* 004 */
	"DENEGADA",	 						/* 005 */
	"RECHAZADA",						/* 006 */
	"RETENER TARJETA",				    /* 007 */
	"RECHAZADA",						/* 008 */
	"RECHAZADA",						/* 009 */
	"RECHAZADA",						/* 010 */
	"APROBADA", 						/* 011 */
	"TRANSACCION INVALIDA",				/* 012 */
	"MONTO INVALIDO",					/* 013 */
	"TARJETA INVALIDA",					/* 014 */
	"RECHAZADA",						/* 015 */
	"RECHAZADA",						/* 016 */
	"RECHAZADA",						/* 017 */
	"RECHAZADA",						/* 018 */
	"RECHAZADA",						/* 019 */
	"RECHAZADA",						/* 020 */
	"RECHAZADA",						/* 021 */
	"RECHAZADA",						/* 022 */
	"RECHAZADA",						/* 023 */
	"RECHAZADA",						/* 024 */
	"NO EXISTE ORIGINAL",				/* 025 */
	"RECHAZADA",						/* 026 */
	"RECHAZADA",						/* 027 */
	"RECHAZADA",						/* 028 */
	"RECHAZADA",						/* 029 */
	"ERROR EN FORMATO",					/* 030 */
	"RECHAZADA",						/* 031 */
	"RECHAZADA",						/* 032 */
	"RECHAZADA",						/* 033 */
	"RECHAZADA",						/* 034 */
	"RECHAZADA",						/* 035 */
	"RECHAZADA",						/* 036 */
	"RECHAZADA",						/* 037 */
	"EXCEDE INGRESO PIN",				/* 038 */
	"ERROR FECHA VENC O COD SEG",		/* 039 */
	"RECHAZADA",						/* 040 */
	"RECHAZADA",						/* 041 */
	"RECHAZADA",						/* 042 */
	"RETENER TARJETA",					/* 043 */
	"RECHAZADA",						/* 044 */
	"NO OPERA EN CUOTAS",				/* 045 */
	"TARJETA NO VIGENTE",				/* 046 */
	"PIN REQUERIDO",					/* 047 */
	"EXCEDE MAXIMO CUOTAS",				/* 048 */
	"ERROR FECHA VENCIMIENTO",			/* 049 */
	"SUPERA LIMITE",					/* 050 */
	"FONDOS INSUFICIENTES",				/* 051 */
	"RECHAZADA",						/* 052 */
	"CUENTA INEXISTENTE",				/* 053 */
	"TARJETA VENCIDA",					/* 054 */
	"PIN INCORRECTO",					/* 055 */
	"TARJETA NO HABILITADA",			/* 056 */
	"TRANSACCION NO PERMITIDA",			/* 057 */
	"SERVICIO INVALIDO",				/* 058 */
	"RECHAZADA",						/* 059 */
	"RECHAZADA",						/* 060 */
	"EXCEDE LIMITE",					/* 061 */
	"RECHAZADA",						/* 062 */
	"RECHAZADA",						/* 063 */
	"RECHAZADA",						/* 064 */
	"EXCEDE LIMITE",					/* 065 */
	"RECHAZADA",						/* 066 */
	"RECHAZADA",						/* 067 */
	"RECHAZADA",						/* 068 */
	"RECHAZADA",						/* 069 */
	"RECHAZADA",						/* 070 */
	"RECHAZADA",						/* 071 */
	"RECHAZADA",						/* 072 */
	"RECHAZADA",						/* 073 */
	"RECHAZADA",						/* 074 */
	"RECHAZADA",						/* 075 */
	"LLAMAR AL EMISOR",					/* 076 */
	"ERROR PLAN CUOTAS",				/* 077 */
	"RECHAZADA",						/* 078 */
	"RECHAZADA",						/* 079 */
	"RECHAZADA",						/* 080 */
	"RECHAZADA",						/* 081 */
	"RECHAZADA",						/* 082 */
	"RECHAZADA",						/* 083 */
	"RECHAZADA",						/* 084 */
	"APROBADA",							/* 085 */
	"RECHAZADA",						/* 086 */
	"RECHAZADA",						/* 087 */
	"RECHAZADA",						/* 088 */
	"TERMINAL INVALIDA",				/* 089 */
	"RECHAZADA",						/* 090 */
	"EMISOR FUERA DE LINEA",			/* 091 */
	"RECHAZADA",						/* 092 */
	"RECHAZADA",						/* 093 */
	"NRO SECUENCIA DUPLICADO",			/* 094 */
	"RE-TRANSMITIENDO",					/* 095 */
	"ERROR EN SISTEMA",					/* 096 */
	"RECHAZADA",						/* 097 */
	"RECHAZADA",						/* 098 */
	"RECHAZADA",						/* 099 */
	"ANULACION PENDIENTE PINPAD",		/* 100 */
	"ESPERA RESPUESTA DEVOLUCION",		/* 101 */
	"TERMINAL NO CONFIGURADA",			/* 102 */
	"EXCEDE LIMITE OFFLINE",			/* 103 */
	"CIERRE LOTE ACTIVO",				/* 104 */
	"ERROR INSERT ORIGINAL",			/* 105 */
	"TARJETA EN BOLETIN",				/* 106 */
	"ERROR EN BASE DE DATOS",			/* 107 */
	"NO EXISTE ORIGINAL",               /* 108 */
	"ERROR HORA ENVIO/TRANSMISION",		/* 109 */
	"ERROR EN ANULACION CASHBACK"		/* 110 */
};

void ImprimirErroresX25()
{
	int iErr = 0;
	int iUbi = 0;
	int iTam = 0;
	printf("\nErrores X25:\n");
	while ( atoi(stX25[iErr].sErr) != 0)
	{
		printf("%03s %-16s\n", stX25[iErr].sErr, stX25[iErr].sEti);
		iUbi = 0;
		iTam = strlen(stX25[iErr].sDet);
		while (iUbi < iTam)
		{
			printf("    %-*.*s\n", TAM_LIN, TAM_LIN, &stX25[iErr].sDet[iUbi]);
			iUbi += TAM_LIN;
		}
		iErr++;
	}
	printf("\n");
}

void ImprimirErroresISO()
{
	int iErr;
	printf("\nErrores ISO:\n");
	for (iErr=0; iErr<MAX_ISO; iErr++)
	{
		printf("%03d %s\n", iErr, stErr[iErr]);
	}
	printf("\n");
}

void ImprimirErroresTCP()
{
	int iErr;
	printf("\nErrores TCP:\n");
	for (iErr=0; iErr<MAX_TCP; iErr++)
	{
		printf("%03d %s\n", iErr, strerror(iErr));
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	printf("errcvc 1.0 Parametro \"x25\" (defecto), \"tcp\" \"iso\"\n");
	if ( (argc==2) && (!strcmp(argv[1], "iso"))  )
		ImprimirErroresISO();
	else
		if ( (argc==2) && (!strcmp(argv[1], "tcp"))  )
			ImprimirErroresTCP();
		else
			ImprimirErroresX25();
	return 0;
}
