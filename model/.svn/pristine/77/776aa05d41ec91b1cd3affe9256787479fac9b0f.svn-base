    (:get list of resource from database:)  
    let $assetResourceList := for $resource in collection('ipvs.default.assetresourcelist') 
							  return $resource
							   
	(:insert bot users element after template asset resource nid:) 	
	let $assetresourceafterupdate := for $resource in $assetResourceList 
									        where $resource[not(.//BotUser)]
								 return update insert <BotUser enable="false"></BotUser> following  $resource//TemplateAssetResourceNID
								 
	(:insert get list of bot users and update password,  bot user information in asset resource doc:)   
	return for $user in collection('ipvs.default.userlist')[.//Groups[Tag[@Name='_RESERVED_']/@Value='DEVICE_BOT_USERS']]
                     let $username := data($user//Username)
                     let $password := data($user//Password)
                     let $replacedAssetResourceKeyData :=  for $resource in $assetResourceList//AssetResource[data(.//Title)=$username]
					                                            where $resource[not(.//StateFlag='Unmanaged')]
															return update replace $resource//KeyData with <KeyData> {$password} </KeyData> 
                     let $insertBotUserInfo :=  for $resource in $assetResourceList//AssetResource[data(.//Title)=$username]
													where $resource[not(.//StateFlag='Unmanaged')]
												return update replace $resource//BotUser with <BotUser enable="true"></BotUser> 
			 return $user

