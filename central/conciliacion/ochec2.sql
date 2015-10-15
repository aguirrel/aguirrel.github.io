--------------------------------------------------------------------------
-- OCHECK2.SQL:  Controla datos cargados desde sistema ON-LINE
-- -----------
--
--     parametros:
--     ----------
--
--         1) Fecha ( YYYYDDMM )
--         2) Sucursal ( SS )
--         3) Fecha carga ( DDMM )
--         4) Tipo procesamiento ( "N" = control primera vez
--                                 "REPROCESAR" = repetir proceso ) 
--
-- NOTA: Si se desea correr el script nuevamente se debe dar "REPROCESAR"
-- ----  como valor del cuarto parametro ya que de lo contrario no toma
--       el total de los registros de la tabla CRED_TRX utilizados en la
--       primer corrida
--
--------------------------------------------------------------------------

set echo off
set timing on
set serveroutput on 

truncate table cred_trx_temp;
truncate table vtacard_temp;

spool /online/getdata/ocheck2.log

declare

    xnrotrx             varchar2(4);
    xtarjeta            varchar2(20);
    xtipo               varchar2(2);
    xmonto              varchar2(15);
    xfechareal          varchar2(8);
    xfecha              varchar2(6);
    xrowid              rowid;
    xtiptar             varchar2(2);
    xnrocaj             varchar2(5);
    xsuc                char(5) := LPAD('&2',5,'0');

    filachk             rowid;
    filachk2            rowid;

    cursor c_vtacard is
    select nro_trx, tarjeta, tipo, monto, fec_vtacard, fecha, tip_tar,
           terminal, row_id
           from vtacard_temp
           where fec_vtacard = '&1';
         
    cursor c_cred is 
    select row_id 
           from cred_trx_temp 
           where nro_caj = xnrocaj and
                 nro_trx = to_number(xnrotrx) and
                 cod_trx = xtipo and
                 monto = to_number(xmonto) * 100;

begin

    update vtacard
    set marca = NULL
    where fec_vtacard = '&1' and
          sucursal='&2';
       
    insert into vtacard_temp
    select nro_trx, tarjeta, tipo, monto, fec_vtacard, fecha,
           tip_tar, terminal, rowid
           from vtacard
           where fec_vtacard = '&1' and
                 sucursal = '&2';   -- and
                                    -- marca is null; 

    commit;

    update cred_trx
    set monto = monto_uni
    where nro_caj_sf=xsuc and
          cod_tar='20' and
          monto_uni is not null;
          
    commit;

    if upper('&4') = 'REPROCESAR'
    then

        update cred_trx
        set fec_vtacard = '0000'
        where nro_caj_sf = xsuc and
              nro_caj||to_char(nro_trx,'9999')||cod_trx||to_char(monto,'999999999.99') in
                   (select terminal||to_char(to_number(nro_trx),'9999')||tipo||
                           to_char(to_number(monto)*100,'999999999.99') from vtacard_temp );

         commit;

    end if;

    insert into cred_trx_temp
    select a.rowid, a.nro_caj, a.nro_trx, a.cod_trx, a.monto
           /*+ index (cred_trx, credtrx_i6) */
           from cred_trx a, vtacard_temp b
           where a.fec_vtacard = '0000' and
                 a.nro_caj_sf = xsuc and
                 a.nro_caj = b.terminal and
                 a.nro_trx = to_number(b.nro_trx) and
                 a.cod_trx = b.tipo and
                 a.cod_est in ( '00','02', '04', '12','43') and
                 a.monto = to_number(b.monto) * 100;

    commit; 

    update cred_trx
    /*+ index (cred_trx,credtrx_i6) */
    set fec_vtacard = ( select decode(cod_est,'43',decode(substr(fec_loc,1,4),substr('&1',7,2)||substr('&1',5,2),'&3'
                                                                                                                 ,NULL)
                                                   ,NULL) 
                               from dual )
    where fec_vtacard = '&3' and
          nro_caj_sf=xsuc;
     
    commit;

    open c_vtacard;

    loop

        fetch c_vtacard into xnrotrx, xtarjeta, xtipo, xmonto, 
                             xfechareal, xfecha, xtiptar, xnrocaj, xrowid;

        exit when c_vtacard%notfound;

        open c_cred; 

        fetch c_cred into filachk;

        if c_cred%found
        then

            fetch c_cred into filachk2;

            if c_cred%notfound
            then    -- Solamente una ocurrencia. Esto es OK  .

                update vtacard
                set marca = '0'
                where rowid = xrowid;

                update cred_trx
                set fec_vtacard ='&3' 
                where rowid = filachk;

            else

                update vtacard
                set marca = '1'
                where rowid = xrowid;

            end if;
  
        else    -- Las que no se encuentran son pendientes.

           update vtacard
           set marca = '3'
           where rowid = xrowid;

        end if;

        close c_cred;

        commit;

    end loop;

    close c_vtacard;

end;

/

spool off

quit;
