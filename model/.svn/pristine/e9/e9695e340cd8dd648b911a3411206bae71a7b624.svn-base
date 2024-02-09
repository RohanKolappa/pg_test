    (:get serviceconfig:)  
    let $serviceconfigitems := for $resource in collection('ipvs.default.serviceconfiglist') 
							  return $resource
							   
	let $response := if (count($serviceconfigitems) != 0) then 
							  let $profilelist := for $resource in collection('ipvs.default.mediastreamprofilelist') 
							                       where $resource[.//StreamType='RTP' and .//HLSConfig/Enable='true']
							  						return $resource
							  let $updateResponse := if (count($profilelist) != 0) then 
							                           for $resource in $serviceconfigitems 
							                            where $resource[not(.//WriterOptions)]									    		         
								                       return update insert <WriterOptions enableHLS='true'></WriterOptions> following  $resource//MulticastIPAddressPool	
								                       else 
							          						 for $resource in $serviceconfigitems 
							          						  where $resource[not(.//WriterOptions)]									    		         
								                  		     return update insert <WriterOptions enableHLS='false'></WriterOptions> following  $resource//MulticastIPAddressPool
							  	return $updateResponse			 

								else '' 	
								 
	return $response

