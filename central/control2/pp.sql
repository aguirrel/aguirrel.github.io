set serveroutput on;
declare
  v_count number(5);
begin
  delete  from cred_trx;
  dbms_output.put_line('jjj '||to_char(SQL%ROWCOUNT));
end;
/
