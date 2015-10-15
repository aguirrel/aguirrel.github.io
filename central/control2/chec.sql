       EXEC SQL
           SELECT nro_tar, cod_aut, monto, nro_lot, mod_env
              INTO :nro_tar, :cod_aut:NULL1, :monto, :nro_lot, :mod_env
              FROM cred_trx
              WHERE nro_caj=:nro_caj and 
                    nro_trx=:nro_trx and
                    cod_tar=:cod_tar and
                    (cod_est='00' or cod_est='02' or cod_est='04');
