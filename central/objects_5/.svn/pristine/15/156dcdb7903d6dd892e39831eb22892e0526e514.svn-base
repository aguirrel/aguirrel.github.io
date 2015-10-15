#ifdef OS_Linux
#pragma pack(1)
#endif

/*
+-----------------+------------------------------------------------------------+
| FUENTE          | Config.C                                                   |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Obtjeto de configuracion                                   |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
#include "Includes.h"
#include "Config.h"

#define         LN_ITEM             1
#define         LN_TOPIC            2
#define         LN_COMMENT          3
#define         LN_EMPTY            4
#define         LN_UNKNOW           20

static int GetCfgTopic(char * pcTopic, char * pcBuffCfg);
static int GetCfgItemValue(char * pcItem, char * pcValue, char * xpcLineCfgFile);
static int CfgClasificaLinea( char * cBuff);

/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Config::Config                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor por defecto de la clase.                       |
|                 | Limpia todos los elementos de la clase                     | 
+-----------------+------------------------------------------------------------+
*/
Config::Config()
{
	cfgName[0]='\0';
	cfgFile = NULL;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Config::Config                                             |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Constructor con el nombre del archivo de configuracion     |
|                 | Setea nombre del elemento configurador (archivo) y lo abre | 
+-----------------+------------------------------------------------------------+
*/
Config::Config(char * strConfigName)
{
	if ((cfgFile=fopen(strConfigName,"rt"))==NULL)
		return ;
    strncpy(cfgName,strConfigName,sizeof(cfgName));
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Config::~Config                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Destructor del configurador.                               |
|                 | Cierra el elemento configurador (archivo)                  | 
+-----------------+------------------------------------------------------------+
*/
Config::~Config()
{
    if (cfgFile != NULL )
		fclose(cfgFile);

    cfgName[0]='\0';
    cfgFile=NULL;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Config::SetCfgName                                         |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Configura el nombre del archivo de configuracion           |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Config::SetCfgName(char * strConfigName)
{
    if (cfgFile != NULL )
    {
        fclose(cfgFile);
        cfgName[0]='\0';
        cfgFile=NULL;
    }
    if ((cfgFile=fopen(strConfigName,"rt"))==NULL)
        return ERR_CFG;  
    strncpy(cfgName,strConfigName,sizeof(cfgName));
	return CFG_FOUND;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Config::GetItem                                            |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Busca el valor del item dentro de la seccion especificada  |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
int Config::GetItem(char * topic, char * item, char * value)
{
    char        cLineCfgFile[120];      /* Linea del archivo de configuracion  */
    char        cCurrentTopic[40];      /* Topic in proces                     */
    char        cCurrentItem[40];       /* Current Configuration item          */
    char        cCurrentValue[80];      /* Valor del elemento                  */
    char *      pcEofMarker;            /* Fin de archivo                      */
    int         rc;                     /* Eof Marker                          */
    int         iTipoLinea;             /* Tipo de linea                       */

    if (cfgFile == NULL )
    {
        *value='\0';
        return CFG_ERR_STP;
    }

    fseek(cfgFile,0,0);
       
    cLineCfgFile[sizeof(cLineCfgFile)-1]='\0';

    for (pcEofMarker=fgets(cLineCfgFile, sizeof(cLineCfgFile)-1, cfgFile);
         pcEofMarker!= 0;
         pcEofMarker=fgets(cLineCfgFile, sizeof(cLineCfgFile)-1, cfgFile))
    {
        iTipoLinea = CfgClasificaLinea(cLineCfgFile);
        switch(iTipoLinea)
        {
            case LN_TOPIC:             /* Linea de definicion de topic */
                GetCfgTopic(cCurrentTopic,cLineCfgFile);
                break;
            case LN_ITEM:              /* Linea de definicion de item */
                if (strcmp(cCurrentTopic , topic)==0)
                {
                    GetCfgItemValue(cCurrentItem, cCurrentValue, cLineCfgFile);
    
                    if (strcmp(cCurrentItem,item)==0)
                    {
                        strcpy(value, cCurrentValue);
                        return CFG_FOUND;
                    }
                }
                break;              
            default:                /* Comentario, invalida, vacia */
                break;
        }
    }
    return CFG_NOTFND;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Config::GetCfgTopic                                        |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Toma el nombre del item corriente. Supone que se valido    |
|                 | que la linea es una linea de definicion de topics          | 
+-----------------+------------------------------------------------------------+
*/
static int GetCfgTopic(char * pcTopic, char * pcBuffCfg)
{
    char *      pcSearch;       /* Puntero busqueda */
    char *      pcCpy;			/* Puntero copia    */


    pcSearch=pcBuffCfg;
    pcCpy=pcTopic;

    while (*pcSearch!='[' && *pcSearch!='\0')
        pcSearch++;

    if (*pcSearch!='[')
    {
        *pcCpy='\0';
        return ERR_CFG;
    }
    pcSearch ++;

    while (*pcSearch!=']' && *pcSearch!='\0')
    {
        *pcCpy = *pcSearch;
        pcSearch++;
        pcCpy++;
    }
    if (*pcSearch!=']')
    {
        *pcTopic='\0';
        return ERR_CFG;
    }
	*pcCpy = '\0';
    return ERR_CFG;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Config::GetCfgItemValue                                    |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Toma el valor del parametro                                |
|                 |                                                            | 
+-----------------+------------------------------------------------------------+
*/
static int GetCfgItemValue(char * pcItem, char * pcValue, char * pcLineCfgFile)
{
    char *      pcSearch;           /* Puntero busqueda */
    char *      pcCpy;              /* Puntero copia    */
    char *      pcSrcItem;
    char *      pcSrcValue;

    pcCpy = strdup(pcLineCfgFile);

    pcSearch=strchr(pcCpy,'=');
    if (pcSearch)
    {
        *pcSearch = '\0';
        for(pcSrcItem=pcCpy;*pcSrcItem==' ' || *pcSrcItem=='\t';pcSrcItem++)
        {
            ;       // empty loop...
        }
        strcpy(pcItem,pcSrcItem);
        pcSearch++;

        for(pcSrcValue=pcSearch;*pcSrcValue==' ' || *pcSrcValue=='\t';pcSrcValue++)
        {
            ;       // empty loop...
        }
        strcpy(pcValue,pcSrcValue);

        pcSearch=pcItem+strlen(pcItem)-1;
        while (pcSearch!=pcItem && (*pcSearch == ' '  || *pcSearch == '\t' || *pcSearch == '\n' ))
        {
            *pcSearch='\0';
            pcSearch--;
        }

        pcSearch=pcValue+strlen(pcValue)-1;
        while (pcSearch!=pcValue && (*pcSearch == ' '  || *pcSearch == '\t' || *pcSearch == '\n' ))
        {
            *pcSearch='\0';
            pcSearch--;
        }
    }
    free(pcCpy);
    
    return CFG_FOUND;
}
/*
+-----------------+------------------------------------------------------------+
| FUNCION         | Config::CfgClasificaLinea                                  |
+-----------------+------------------------------------------------------------+
| DESCRIPCION     | Determina el tipo de linea leida del archivo ASCII         |
|                 | Valores posibles:                                          | 
|                 | 1. Definicion de topic.                                    |
|                 | 2. Definicion de item.                                     |
|                 | 3. En blanco.                                              |
|                 | 4. Comentario.                                             |
|                 | 5. Incorrecta.                                             |
+-----------------+------------------------------------------------------------+
*/
static int CfgClasificaLinea( char * cBuff)
{
    char *      pcCpyBuff;          /* Buffer de trabajo.             */
    char *      pcSearchComment;    /* Puntero para busq, de coments. */
    char *      pcSearchEq;
    char *      pcSearchLeftBr;
    char *      pcSearchRightBr;
    char *      pcSearchWord;
    int         iTpoLinea;

    pcCpyBuff=strdup(cBuff);

    pcSearchComment = strchr(pcCpyBuff,';');

    if (pcSearchComment)
        *pcSearchComment='\0';

    /* Determina si puede ser un item */
    pcSearchComment = strchr(pcCpyBuff,'=');
          
    if (pcSearchComment)
    {
		for (pcSearchWord=pcCpyBuff;*pcSearchWord!='=';pcSearchWord++)
            if (isalpha(*pcSearchWord))
                break;

        iTpoLinea =  *pcSearchWord != '=' ? LN_ITEM : LN_UNKNOW;
    }
    else
    {
        pcSearchLeftBr =strchr(pcCpyBuff,'[');
        pcSearchRightBr=strchr(pcCpyBuff,']');
                
        if (pcSearchLeftBr &&  pcSearchRightBr )
        {
            if (pcSearchLeftBr <  pcSearchRightBr)
            {
                for ( pcSearchWord=pcSearchLeftBr;
                      *pcSearchWord != ']';
                      pcSearchWord++)
                    if (isalpha(*pcSearchWord))
                        break;
                iTpoLinea =  *pcSearchWord != '=' ? LN_TOPIC : LN_UNKNOW;
            }
            else
            {
                iTpoLinea = LN_UNKNOW;
            }
        }
        else
        {
            for (pcSearchWord=pcCpyBuff; *pcSearchWord!='\0';pcSearchWord++)
            {
                if (*pcSearchWord != ' ' && *pcSearchWord != '\t' &&  *pcSearchWord != '\n') 
                    break;
                if (*pcSearchWord != '\0')
                    iTpoLinea = LN_UNKNOW;
                else
                    iTpoLinea = pcSearchComment ? LN_COMMENT : LN_EMPTY;
            }
        }
    }
    free(pcCpyBuff);
    return iTpoLinea;
}

