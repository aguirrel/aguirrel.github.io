#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef TRACENUMBER_H
#define TRACENUMBER_H

#include "Includes.h"
#include "Table.h"

class TraceNmbr
{
protected:
   int		ret;
   long		trace_number;
   char		aux_str[7];
   TraceData    trace_data;
   TraceTable   DBTrace;

public:
   TraceNmbr();
   ~TraceNmbr();

   void SetNroCA (char *nro_ca);
   void SetNroSuc(char *nro_suc);
   void SetNroCaj(char *nro_caj);
   void SetCodConCie(char *cod_con_cie);
   operator long();
   operator char*();
   char *operator ++();
   char	*GetIsoStr();
   long	Inc();
};

#endif

