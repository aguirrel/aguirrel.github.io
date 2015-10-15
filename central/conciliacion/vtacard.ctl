load data
append
into table vtacard
( terminal position(1:5) char,
  nro_trx  position(6:9) char,
  tip_tar  position(10:11) char,
  tarjeta  position(12:31) char,
  monto    position(40:53) char,
  fecha    position(54:59) char,
  tipo     position(60:61) char,
  fec_vtacard     position(77:84) char,
  sucursal     position(85:86) char
  )

