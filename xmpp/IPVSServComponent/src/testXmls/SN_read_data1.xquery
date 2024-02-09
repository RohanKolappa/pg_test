 for $b in  collection("ipvsCol_THREADNUMBER_")//MediaStreamFileResource
 	where $b//MetaData/Tag[@Name='Physician']='abc_LOOPNUMBER_'
 	 and $b//MetaData/Tag[@Name='Patient']='jack_THREADNUMBER_'
 	
 return $b
