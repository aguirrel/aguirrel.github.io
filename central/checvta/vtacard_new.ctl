load data
into table vtacard_dia_new (
nro_caj  position(01:05) char,
nro_trx  position(06:09) char,
cod_tar  position(10:11) char,
nro_tar  position(12:31) char,
cod_aut  position(32:37) char,
monto    position(40:53) char,
fec_trx  position(54:59) char,
cod_trx  position(60:61) char,
hora     position(68:71) char,
plan_sf  position(72:73) char,
nro_cuo  position(74:76) char,
nro_caj_sf position(85:87) char
)

