 for $b in  collection("ipvsCol_THREADNUMBER_")//MetaData[ (Tag/@Name='Physician'  and Tag='abc_LOOPNUMBER_') and (Tag/@Name='Patient' and Tag='jack_THREADNUMBER_')]
 return $b
