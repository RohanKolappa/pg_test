Organization of code
====================
StartRecordingRequestHandler
--------------------------
	- Sends a CreateRecordingRequest to the AppServer.
	
StopRecordingRequestHandler         
--------------------------
    - Sends a DeleteMediaRoomRequest to the AppServer.
      
ProxyPauseResumeBaseRequestHandler
---------------------------------
	- Implements a method that sends AddTrackActionRequest to the AppServer.
      
PauseRecordingRequestHandler
-------------------------------
    - Extends the ProxyPauseResumeBaseRequestHandler and calls the method from the base class that sends AddTrackActionRequest to the AppServer.

ResumeRecordingRequestHandler
-------------------------------
     - Extends the ProxyPauseResumeBaseRequestHandler and calls the method from the base class that sends AddTrackActionRequest to the AppServer.
        
RecordingStatusRequestHandler
----------------------------
     - Sends a GetSrcMediaFileListRequest to the AppServer.

UpdateRecordingRequestHandler
-----------------------------
     - Sends a UpdateDestFileMetaDataRequest to the AppServer.

CreateBookmarkRequestHandler 
----------------------------
     - Send a AddBookmarkRequest to the AppServer.      

   
AuthorizationUpdateRequestHandler 
--------------------------------
     - Calls a revokeXAuth method from the XAuthServerAgent which send a xauth revoked message to the AppServer.      
 
GetStreamListRequestHandler
---------------------------
     - Returns list of streams from the cache. The cache is maintained in the XAuthServerAgent.      
  
      
         

         
         
     
   
    
 
       
         
     
                
       
          
                
                    
                
         
        
 
