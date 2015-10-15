CREATE INDEX ind_credtrx_con
on cred_trx(fec_trx,nro_caj_ca,cod_est)
tablespace it601001
storage (initial 500k next 1M pctincrease 0)
/
