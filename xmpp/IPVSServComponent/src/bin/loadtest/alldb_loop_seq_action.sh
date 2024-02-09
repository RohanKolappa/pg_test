#!/bin/sh
createCount=6
readCount=2
readUpdateCount=3
logprefix=alltest_

curlog=test.log
cursumlog=summary.log
rm -rf ${curlog}
rm -rf ${cursumlog}


for i in 1 4 8 16 32
do
printf "\n\n\n______________________________# ${i} START______________________________________________"
    ./run_test_alldb.sh testdb_deletealldb.xml ${i} 1  ${logprefix}DA_
    
    ./run_test_alldb.sh testdb_createdb.xml ${i} ${createCount} ${logprefix}C_
    
    ./run_test_alldb.sh testdb_readdb.xml ${i} ${readCount} ${logprefix}R_
    
    ./run_test_alldb.sh testdb_readandupdate.xml ${i} ${readUpdateCount} ${logprefix}RU_
 
    ./run_test_alldb.sh testdb_deletedocdb.xml ${i} ${createCount} ${logprefix}DD_
    
    ./run_test_alldb.sh testdb_deletecol.xml ${i} 1  ${logprefix}DC_
printf "______________________________# ${i} START______________________________________________\n\n\n"
    
    
    
done

