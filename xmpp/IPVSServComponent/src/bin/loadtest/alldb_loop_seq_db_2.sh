#!/bin/sh
createCount=10
readCount=3
readUpdateCount=3
deleteCount=10
createCollectionCount=1
deleteCollectionCount=1

logprefix=alltest_
loopLimit=16
loopInterval=4

curlog=test.log
cursumlog=summary.log
rm -rf ${curlog}
rm -rf ${cursumlog}
rm -rf *.log

for i in 1 4 8 16  
#for ((  i = 1 ;  i <= ${loopLimit};  i=`expr $i + $loopInterval` ))
do
	test_db_name="sednaxmldbtest"
	echo ".............................. ${i}  ...................." 
	echo ".............................. ${i}  ...................." >> ${cursumlog}

	echo "............................ $test_db_name ${i}  ...................." 
	echo "............................ $test_db_name ${i}  ...................." >> ${cursumlog}

    ./run_test_db.sh $test_db_name testdb_deletealldb.xml ${i} ${createCollectionCount}  ${logprefix}DA_
    
    ./run_test_db.sh $test_db_name testdb_createdb.xml ${i} ${createCount} ${logprefix}C_
    
    ./run_test_db.sh $test_db_name testdb_readdb.xml ${i} ${readCount} ${logprefix}R_
    
    ./run_test_db.sh $test_db_name testdb_readandupdate.xml ${i} ${readUpdateCount} ${logprefix}RU_
 
    ./run_test_db.sh $test_db_name testdb_deletedocdb.xml ${i} ${deleteCount} ${logprefix}DD_
    
    ./run_test_db.sh $test_db_name testdb_deletecol.xml ${i} ${deleteCollectionCount}  ${logprefix}DC_
    
    test_db_name="existdbtest"
    echo "" 
    echo "">> ${cursumlog}
	echo "............................. $test_db_name ${i}  ...................."  
	echo "............................. $test_db_name ${i}  ...................." >> ${cursumlog}
    ./run_test_db.sh $test_db_name testdb_deletealldb.xml ${i} ${createCollectionCount}  ${logprefix}DA_
    
    ./run_test_db.sh $test_db_name testdb_createdb.xml ${i} ${createCount} ${logprefix}C_
    
    ./run_test_db.sh $test_db_name testdb_readdb.xml ${i} ${readCount} ${logprefix}R_
    
    ./run_test_db.sh $test_db_name testdb_readandupdate.xml ${i} ${readUpdateCount} ${logprefix}RU_
 
    ./run_test_db.sh $test_db_name testdb_deletedocdb.xml ${i} ${deleteCount} ${logprefix}DD_
    
    ./run_test_db.sh $test_db_name testdb_deletecol.xml ${i} ${deleteCollectionCount}  ${logprefix}DC_
    
    test_db_name="sednanativedbtest"
    echo "" 
    echo "">> ${cursumlog}
	echo "............................. $test_db_name ${i}  ...................." 
	echo "............................. $test_db_name ${i}  ...................." >> ${cursumlog}
    ./run_test_db.sh $test_db_name testdb_deletealldb.xml ${i} ${createCollectionCount}  ${logprefix}DA_
    
    ./run_test_db.sh $test_db_name testdb_createdb.xml ${i} ${createCount} ${logprefix}C_
    
    ./run_test_db.sh $test_db_name testdb_readdb.xml ${i} ${readCount} ${logprefix}R_
    
    ./run_test_db.sh $test_db_name testdb_readandupdate.xml ${i} ${readUpdateCount} ${logprefix}RU_
 
    ./run_test_db.sh $test_db_name testdb_deletedocdb.xml ${i} ${deleteCount} ${logprefix}DD_
    
    ./run_test_db.sh $test_db_name testdb_deletecol.xml ${i} ${deleteCollectionCount}  ${logprefix}DC_
    
    #give some gap. exist has gap , sedna server doe not have. 
    sleep 5
    
    echo "						------" 
    echo "						------" >>${cursumlog}
    echo "						------" 
    echo "						------" >>${cursumlog}
    
    
done


 