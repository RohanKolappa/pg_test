Organization of code
====================
MediaStoreAdminServerAgent
--------------------------
	- handles the request and responses coming to Media Store 
	- handles the system updates (media store condig update like nas entry updates) and convert them as store requests
	- handles the streaming server deleter port progress notificationa and update the dir size in database
	- handles the pub sub dir update events and convert them as store requests
	- Maintains the MediaStoredir Information (Like how many dir are associate to this media store and each dir related information)
	
MediaStoreServerI
-----------------
     - Which will do the actual disk operation. (All read,write and delete operations are going through this class only)
  
  Subclasses:    
     - MediaStoreServerImpl - Implements the common methods associate to all sub class.
         |-> DummyMediaStoreServerImpl - Implements the dummy method which will run in Device simulation Mode          
         |-> DeviceMediaStoreServerImpl - Implements the Actual device disk operations         
         |-> Device2dot4MediaStoreServerImpl - Which will access the 2.4 and generate the latest media store file corresponding to 2.x media.
         |-> Device3dot2MediaStoreServerImpl - Which will access the 3.2 and generate the latest media store file corresponding to 3.2 media.
         
     
        
MediaStoreDirectoryInfo
------------------------
       - Holds the dir information 
       
Export and Import Desing Plan
-----------------------------

           Three Cases:  
              case 1:  New Dir     No Media, No tgz         Set the dir state as Ready 
              case 2:  New Dir     Media There, No tgz      Set the dir state as Need Import
              case 3:  New Dir     Media There, tgz there   Set the dir state as Need Import
              
              Above three cases we have to implement
              
              case 4:  Old Dir     No Media,No tgz                               Existing code is taking care of this case
              case 5:  Old Dir     Media there in blob Not there in db, No tgz   How to handle this case
              case 6:  Old dir     Media There, tgz there                        Set the dir state as Need Import (Similar to above case 3)
              
               

           Modification SynchMediaStoreDirRequestHandler:

             - When creating the dir in the database 
                   Check the list of files or tgz is there in the blob or not. 
                   If either tgz or list of files are there then set the dir state as Need Import in local cache
                   and update the same state in database
                   
             - Dir already created in database.
                    Check the tgz file is there in blob or not. If the tgz file is there then set the dir state as Need Import.
            	    Question: How to figure out that the dir need import when the user went offline and come back online 
            	    when there is no tgz in blob. 
            	    case: User Created dir and set the dir state as Need Imported. With out importing the blob user rebooted the box.
            	    Now appserver sets the dir state as offline. 
            	    Once DMS user is up how to figure out i have to set the dir state as Need Import. 
            	    At this time user start the auto synch process. How to avoid that?
            	    
            	    Note: This part of code is modified in trunk. Need to verify when merging to trunk
            	    
            	    
     
          New Handler ImportMediaStoreHandler in media store service:

               - This request will come from appserver.
               - When appserver asked this request check what kind of import do we need to do
                 means Check tgz is there then ask the appserver to imprort this tgz file. 
                          If no tgz then send the getMediaStreamFileMaxRevison request to appserver. 
                          (Use MediastoreAdminServerAgent sendGetMediaStreamFileMaxRevison Method)
                          This will start synch process. We no need to modify any code related to that.
                          Existing synch logic will take care of this.
                          In both the cases we have to set the dir state as Either Busy or new state called Importing.
            
