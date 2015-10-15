INSERT INTO vtatot  
   ( SELECT SUM(DECODE(tipo,'00',monto,-monto)),
	    SUM(DECODE(marca,'3',DECODE(tipo,'00',monto,-monto),0)),
            terminal,
	    sucursal,
	    tip_tar,
	    fec_vtacard
       FROM vtacard
       WHERE fec_vtacard='&1'
       GROUP BY terminal,sucursal,tip_tar,fec_vtacard)
/
       COMMIT;
/
QUIT;
