#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | TraceNmbr.C                                                |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Objeto para el manejo del numero de trace                  |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Log.h"
#include "TraceNmbr.h"

extern Log	LogAlarm;

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::TraceNmbr                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor del objeto                                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TraceNmbr::TraceNmbr()
{
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::~TraceNmbr                                      |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destructor del objeto                                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TraceNmbr::~TraceNmbr()
{
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::SetNroCA                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Setea el numero de centro autorizador                      |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void TraceNmbr::SetNroCA(char *nro_ca)
{
    trace_data.NroCA(nro_ca);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::SetNroSuc                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Setea el numero de empresa                                 |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void TraceNmbr::SetNroSuc(char *nro_suc)
{
    trace_data.NroSuc(nro_suc);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::SetNroCaj                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Setea el numero de caja                                    |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void TraceNmbr::SetNroCaj(char *nro_caj)
{
    trace_data.NroCaj(nro_caj);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::SetCodConCie                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Setea el codigo de condicion de cierre                     |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
void TraceNmbr::SetCodConCie(char *cod_con_cie)
{
    trace_data.CodConCie(cod_con_cie);
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::operator long                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el numero de trace en formato entero largo         |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TraceNmbr::operator long()
{
    DBTrace.Select(trace_data.GetSelData());
    trace_data.PutSelData();
    trace_number=trace_data.NroTrc();
    return trace_number;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::operator char *                                 |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el numero de trace en formato cadena               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
TraceNmbr::operator char*()
{
    return GetIsoStr();
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::operator ++                                     |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Incrementa el numero de trace                              |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
char *TraceNmbr::operator ++()
{
    GetIsoStr();
    Inc();
    return aux_str;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::GetIsoStr                                       |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtiene el numero de trace en formato cadena               |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
char *TraceNmbr::GetIsoStr()
{
    ret=DBTrace.Select(trace_data.GetSelData());
    if (ret==NOOK)
    {
		LogAlarm.Put(0, "Error obteniendo numero de trace actual (%d)!!\n",
	    DBTrace.GetErrno());
		return "";
    }
    trace_data.PutSelData();
    trace_number=trace_data.NroTrc();
    LogAlarm.Put(99, "TraceNmbr::GetIsoStr(): Trace: '%06ld'\n", trace_number);
    sprintf(aux_str, "%06ld", trace_number);

    return aux_str;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | TraceNmbr::Inc                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Incrementa el numero de trace                              |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
long TraceNmbr::Inc()
{
    DBTrace.Select(trace_data.GetSelData());
    trace_data.PutSelData();
    trace_number=trace_data.NroTrc();
    if (++trace_number>999999)
		trace_number=0;

    trace_data.NroTrc(trace_number);
    DBTrace.Update(trace_data.GetUpdData());
    DBTrace.Commit();

    return trace_number;
}

