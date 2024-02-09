


1- checkout projects 
checkout mediaroom, xmpp/IPVSServComponent, xmpp/mediaroomplugin
folder structure should be like this
+PROJECT_FOLDER
	+mediaroom
	+xmpp
		+IPVSServComponent
		+mediaroomplugin



2- copy mediaroomplugin and ipvscomponent distributions into 'home' folder
	folder structure
	../PROJECT_FOLDER/xmpp/mediaroomplugin/home
	+xmpp
		IPVSServComponent
		+mediaroomplugin
			+home
				+ onair/ipvscomponent (copy ipvscomponent build distributions)
				+ onair/mscomponent (copy mscomponent build distributions)
	C:/home/onair/modules/mediaroom/public (copy mediaroom build distributions)

	two ways to copy 
	a- from ms 
		/home/onair/ipvscomponent to ../PROJECT_FOLDER/xmpp/mediaroomplugin/home/onair/ipvscomponent
		/home/onair/mscomponent to ../PROJECT_FOLDER/xmpp/mediaroomplugin/home/onair/mscomponent
	b- copy from build area after make or run ant
		../PROJECT_FOLDER/xmpp/IPVSServComponent/target/ipvscomponent to ../PROJECT_FOLDER/xmpp/mediaroomplugin/home/onair/ipvscomponent
		../PROJECT_FOLDER/xmpp/mediaroomplugin/target/mscomponent to ../PROJECT_FOLDER/xmpp/mediaroomplugin/home/onair/mscomponent

3- 	edit configuration - FILE home/onair/mscomponent/conf/mscomponent/mscomponent.xml
	-change all localhost to SERVER_IP 
	
5- java arguments
	got to xmpp\mediaroomplugin project from eclipse
	open debug configrations from eclipse for com.ipvs.xmpp.client.component.MSComponentModule
	and copy following arguments to VM arguments tab
	-DipvsConfigFilePath=home/onair/mscomponent/conf/mscomponent/mscomponent.xml 
	-Djava.endorsed.dirs=home/onair/ipvscomponent/xmldblib/sedna/endorsed 
	-DipvsAppName=mscomponent 
	-DipvscomponentHome=home/onair/ipvscomponent  
	-Dipvscomponent.lib.dir=home/onair/ipvscomponent/lib   
	-Djava.security.egd=file:/dev/./urandom  
	-DipvsConfigFilePath=home/onair/mscomponent/conf/mscomponent/mscomponent.xml
	-Dlog4jConfigFilePath=home/onair/mscomponent/conf/mscomponent/log4j.xml
		

4-run from eclipse
	got to xmpp\mediaroomplugin project from eclipse
	and run main method of 
	com.ipvs.xmpp.client.component.MSComponentModule

4- attach project references to debug live -- like mediaroom , xmpp/IPVSServComponent 
	right click -> build path -> configure build path -> projects tab 
	add reference to mediaroom project
	add reference to IPVSServComponent project
	  