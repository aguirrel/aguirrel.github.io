/************************************************************************/
/*                                                                      */
/* Price.C          : Definicion de la clase Price                      */
/*                                                                      */
/* Proyecto         : Autorizacion de tarjetas de credito - StoreFlow.  */
/* Cliente          : Coto                                              */
/* Autor            : Lalo Steinmann                                    */
/* Fecha            : 17/02/95                                          */
/* Compilador       : C++                                               */
/* Base de datos    : Oracle                                            */
/*                                                                      */
/************************************************************************/

#include "Price.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


// Funciones de tratamiento de conversion de distintos tipos de anulaciones


//Funcion Auxiliar que convierte la fecha de Juliano a Gregoriano

int Price_Msg::ObtVersion(char * data)
{
   return VER22;
}

int Price_Msg::ObtVersion()
{
   return VER22;
}
int Price_Msg::ObtTipoMsg(char* data)
{
    char aux_str[10];
    
    strncpy(aux_str,&data[31],4);
    aux_str[4]=0;

    return atoi(aux_str);
}

int Price_Msg::ObtTipoRta(char* data)
{
    char aux_str[10];
    
    strncpy(aux_str,&data[35],4);
    aux_str[4]=0;
    return atoi(aux_str);
}

Price_Msg::Price_Msg()
{
    type=NULL;
    version=0;
    int i=0;
    for (i=0;i<MAX_CAMPOS_PRICE;i++)
         Buffer[i][0]=0; 
}

Price_Msg::Price_Msg(int msg_type)
{
    char aux_str[10];
 
    type=msg_type;
    version=0;
    int i=0;
    for (i=0;i<MAX_CAMPOS_PRICE;i++)
         Buffer[i][0]=0; 
    switch (msg_type)
    {
        case TYPE_PRICE_VENTA:
        case TYPE_PRICE_VENTA_LOCAL:
        case TYPE_PRICE_VENTA_LOCAL_BIS:
        case TYPE_PRICE_ANUL_VENTA:
        case TYPE_PRICE_ANUL_VENTA_BIS:
           sprintf(aux_str,"%04d",msg_type);
           PutField(3,aux_str);
           break;
        case TYPE_PRICE_VENTA_RTA:
        case TYPE_PRICE_VENTA_LOCAL_RTA:
        case TYPE_PRICE_ANUL_VENTA_RTA:
           sprintf(aux_str,"%04d",msg_type);
           PutField(5,aux_str);
           break;
    }
}
            


Price_Msg::Price_Msg(str_price_msg price_msg) 
{
   int i;
   int found;
   int j;
   char aux_str[128];
   char data[500];
   int desp;

   for (i=0;i<MAX_CAMPOS_PRICE;i++)
         Buffer[i][0]=0; 
   memcpy(data,(char *)&price_msg,sizeof(price_msg));

   version=ObtVersion(data);
   type=ObtTipoMsg(data);

   i=0;
   found=0;
   while ((FormCamposPrice[i].version!=0) &&
         ((FormCamposPrice[i].version!=version) || 
          (FormCamposPrice[i].tipo_msg!=type)))
       i++;

   
   if (FormCamposPrice[i].version==0)
      return;

   j=0;
   desp=0;

   while (j<FormCamposPrice[i].CanCampos)
   {
       
       memset(aux_str,0x00,sizeof(aux_str));
       strncpy(aux_str,&data[desp],FormCamposPrice[i].order_campos[j].longi);

       desp=desp+FormCamposPrice[i].order_campos[j].longi;

	   PutField(FormCamposPrice[i].order_campos[j].campo,aux_str);
       j++;
   }

   return;
}


Price_Msg::Price_Msg(str_price_msg_rta price_msg_rta) 
{
   int i;
   int found;
   int j;
   char aux_str[70];
   char data[500];
   int  desp=0;

   memcpy(data,(char *)&price_msg_rta,sizeof(str_price_msg_rta));

   version=ObtVersion(data);
   type=ObtTipoRta(data);

   i=0;
   found=0;
   while ((FormCamposPrice[i].version!=0) &&
         ((FormCamposPrice[i].version!=version) || 
          (FormCamposPrice[i].tipo_msg!=type)))
       i++;
   
   if ((FormCamposPrice[i].version!=version) ||
      (FormCamposPrice[i].tipo_msg!=type))
      return;

   for (j=0 ; j < FormCamposPrice[i].CanCampos ;j++ )
   {
       memcpy(aux_str,0x00,sizeof(aux_str));
       strncpy(aux_str,&data[desp],FormCamposPrice[i].order_campos[j].longi);
       desp=desp+FormCamposPrice[i].order_campos[j].longi;
       PutField(FormCamposPrice[i].order_campos[j].campo,aux_str);
   }

   return;
}

int Price_Msg::PutField(int nro,char *data)
{
   strcpy(Buffer[nro],data);
   return OK;
}

char* Price_Msg::GetField(int nro)
{
   static char data[500];

   strcpy(data,Buffer[nro]);
   return data;
}

char* Price_Msg::GetNroTar()
{
    static char aux_str[500];
    int cont;
    strcpy(aux_str, GetField(4));

	 
    /* Eliminar los blancos en el campo nro_tar del msg. price. */
    cont=0;
    while ( (aux_str[cont] == ' ') && (cont < (int)strlen(aux_str)) )
	   cont++;
    while ( (aux_str[cont]!=' ') && (cont < (int)strlen(aux_str)) )
           cont++;
    if (aux_str[cont]==' ')
           aux_str[cont]='\0';

    if ((aux_str[0]!=0) && (aux_str[0]!=' '))
        return 	aux_str;
    else
    {
        strcpy(aux_str,ObtTarTrk2(GetField(10)));
		return aux_str;
    }
}


int Price_Msg::GetMsgString(char *msg_string)
{
   int i,j;
   int LonTot;
   char aux_str[50];
   char aux_str2[50];
   int found;


   version=ObtVersion();

   i=0;
   found=0;
   while ((FormCamposPrice[i].version!=0) &&
         ((FormCamposPrice[i].version!=version) || 
          (FormCamposPrice[i].tipo_msg!=type)))
       i++;
   
   if (FormCamposPrice[i].version==0)
      return NOOK;


   msg_string[0]=0;
   LonTot=0;
   for (j=0;j<FormCamposPrice[i].CanCampos;j++)
   {
       strcpy(aux_str,GetField(FormCamposPrice[i].order_campos[j].campo));
       sprintf(aux_str2,"%*.*s",FormCamposPrice[i].order_campos[j].longi,
             FormCamposPrice[i].order_campos[j].longi,aux_str);
       strcat(msg_string,aux_str2);
       LonTot=LonTot+FormCamposPrice[i].order_campos[j].longi;
   }
   
   return LonTot;
}


// Funcion que obtiene el numero de tarjeta a partir del track2
char *Price_Msg::ObtTarTrk2(char * t2)
{
    static char aux_str1[30];
    int cont;

    //Obtener el numero de tarjeta del track2
    strcpy(aux_str1, GetField(10)); 
    cont=0;
    while ( (cont < (int)strlen(aux_str1)) && (aux_str1[cont]!='D') && 
            (aux_str1[cont]!='=') )
            cont++;
    aux_str1[cont]=0;
    return aux_str1;
}

char *Price_Msg::GetTerminal()
{
    static char         aux_str[50];
    switch(type)
    {
        case TYPE_PRICE_VENTA:
        case TYPE_PRICE_VENTA_LOCAL:
        case TYPE_PRICE_VENTA_LOCAL_BIS:
            strcpy(aux_str, GetField(16));
            break;
        case TYPE_PRICE_ANUL_VENTA:
        case TYPE_PRICE_ANUL_VENTA_BIS:
            strcpy(aux_str, GetField(13));
            break;
        case TYPE_PRICE_VENTA_RTA:
        case TYPE_PRICE_VENTA_LOCAL_RTA:
        case TYPE_PRICE_ANUL_VENTA_RTA:
            strcpy(aux_str, GetField(3));
            break;
    }
    return aux_str;
}

char *Price_Msg::GetSucursal()
{
    static char         aux_str[50];
    switch(type)
    {
        case TYPE_PRICE_VENTA:
        case TYPE_PRICE_VENTA_LOCAL:
        case TYPE_PRICE_VENTA_LOCAL_BIS:
            strcpy(aux_str, GetField(12));
            break;
        case TYPE_PRICE_ANUL_VENTA:
        case TYPE_PRICE_ANUL_VENTA_BIS:
            strcpy(aux_str, GetField(10));
            break;
        default:
            strcpy(aux_str, "");
    }
    return aux_str;
}

char *Price_Msg::GetCodTar()
{
    static char         aux_str[50];
    switch(type)
    {
        case TYPE_PRICE_VENTA:
        case TYPE_PRICE_VENTA_LOCAL:
        case TYPE_PRICE_VENTA_LOCAL_BIS:
            strcpy(aux_str, GetField(17));
            break;
        case TYPE_PRICE_ANUL_VENTA:
        case TYPE_PRICE_ANUL_VENTA_BIS:
            strcpy(aux_str, GetField(14));
            break;
        default:
            strcpy(aux_str, "");
    }
    return aux_str;
}


/* Funcion: la cual  controla que esta correcto el campo TrackII del Msg. price */

int Price_Msg::ValidoTrackII (char *campo)  
{
   int  i;
  
  if (campo[0] == ' ' )
  {  // Esta vacio el campo.
     return NOOK;
  }

  i=0;
  while ( (campo[i] != '=') && (campo[i] != 'D') && (i < (int)strlen(campo)) )
    i++;

  if (i == (int)strlen(campo))
  {
     return NOOK;
  }

  return OK;
}

void Price_Msg::InvertirFecha()
{ 
    char fec_aux[5];
    char fec_fin[5];
    char aux_cfg[5];

    aux_cfg[0]=0;
	strcpy(aux_cfg,"N");

    // Se da vuelta si se indica en el archivo de configuracion o
    // si el ingreso fue por banda magnetica

    if ((!strcmp(aux_cfg,"S")) || (!strcmp(GetField(9),"0")))
    {
      strcpy(fec_aux,GetField(8));
      fec_fin[0]=fec_aux[2];
      fec_fin[1]=fec_aux[3];
      fec_fin[2]=fec_aux[0];
      fec_fin[3]=fec_aux[1];
      fec_fin[4]=0;
      PutField(8,fec_fin);
    }
}

void Price_Msg::SetCodSeg()
{
   UseCodSeg=1;
}

void Price_Msg::SetNoCodSeg()
{
   UseCodSeg=0;
}

char *Price_Msg::GetCodSeg()
{
   static char cod_seg[10];
   char  mod_ing[5];
   char  aux_str[50]; 
   
   strcpy(mod_ing,GetField(9));

   if (((!strcmp(mod_ing,"1")) || (!strcmp(mod_ing,"3"))) && (UseCodSeg==1))
   {
      strcpy(aux_str,GetField(10));
      strcpy(cod_seg,&aux_str[34]);
   }
   else
   {
      cod_seg[0]=0;
   }

   return cod_seg;
}

 

char *Price_Msg::ObtPlanSf()
{
   static char plan[3];
   if (version==VER22)
   {
      switch (type) 
      {
         case TYPE_PRICE_VENTA:
            strcpy(plan,GetField(22));
            break;
         case TYPE_PRICE_VENTA_LOCAL:
         case TYPE_PRICE_VENTA_LOCAL_BIS:
            strcpy(plan,GetField(25));
            break;
         default:
            strcpy(plan,"00");
      }
      return plan;
   }
   strcpy(plan,"00");
   return plan;
}


char *Price_Msg::ObtCuotas()
{
   static char cuotas[5];


   if (version==VER22)
   {
      switch (type) 
      {
         case TYPE_PRICE_VENTA:
            if (!strcmp(GetField(23),"000"))
               strcpy(cuotas,"001");
            else 
               strcpy(cuotas,GetField(23));
            break;
         case TYPE_PRICE_VENTA_LOCAL:
         case TYPE_PRICE_VENTA_LOCAL_BIS:
            if (!strcmp(GetField(26),"000"))
               strcpy(cuotas,"001");
            else
               strcpy(cuotas,GetField(26));
            break;
         default:
            strcpy(cuotas,"001"); 
      }
      return cuotas;
   }
   else
   {   
		strcpy(cuotas,"001");
		return cuotas;
   }
}
