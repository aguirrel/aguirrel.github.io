BEGIN
   DELETE vtacard where fec_vtacard='&1' and SUCURSAL='&2';
   COMMIT;
END;
/
QUIT

