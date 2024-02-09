Organization of code
====================
SynchMediaStoreDirRequest
--------------------------
   Handles four Cases.   
     1. Available
     2. UnAvailable
     3. Delete
     4. State Updates
     
     1. Available : 
        ----------
         Service Version checks. Check Media Store Service Version with System Software Version
        
            --> Matches, Checks the dir is there in the database or not. 
                If not add the dir in the system and start the synch.
                If the dir there in the database, checks the dir has the media store entry or not.
                If the entry is not there then add the entry and update it in database.
                If the entry is there then update the dir with latest information.
                Start the synch(Send GetMediaStreamFileMaxRevisionrequest to appserver) only if the dir state is Error or Offline.
                Checks the state of the dir is Busy then set the local Media Store State as BUSY and wait for state update event.
                                                
            --> Mismatches, Add the dir as read only if the dir is not there in the database. 
                If the dir is there in the database then update the dir as read only if the dir is not in read only mode. 
                Start the synch process (Send GetMediaStreamFileMaxRevisionrequest request to appserver).
                
     2. Unavailable :
        -------------
        Make the Media Store presence as unavailable in the dir. If no other Store ports are available then make the dir state as OFFLINE.
        
     3. Delete:
        -------        
        Removes Media Store Entry from Dir.
        
     4. State Updates: 
        --------------
        Checks the dir state is Offline or Error then start the synch (Send GetMediaStreamFileMaxRevisionrequest to appserver). 
        (When some other user us synch the dir then suddenly synch terminates then appserver updates the dir state as Offline)
        If the mediastore state is busy and the dir state is Ready then set the store state as ready and send the getmediaroominvites request.
        (When media store is waiting for synch to complete the store state going to be busy once other person compeltes his synch then the media store also changes his state lcoally)
        
GetMediaStreamFileMaxRevisionResponse:
--------------------------------------
     1. Checks the Media Stream File Max Revision with Store revision 
        - If the revision is matching update the dir state as ready and send GetMediaRoomInvitesRequest to appserver.
        - If the revisions are not matching then start the media stream file synch.
        - If the dir in read only mode then synch the entire store.
        - If the revisions are mismatching then synch the files that are higher than media stream file max revision
        - First synch the blob and then start the synch older media groups and play list (2.x in db dir).
        - When synch'ing if the Media Store file service version is not matching with system software version then mark the file as read only in the database.
          (Older service version media synch)
     
     2. Repair the files which are in Busy/Error/NotReady State.
     
     3. Compare the all the MediaStreamFiles in the database are there in the store or not. If not then delete those files from database.
     
      
         

         
         
     
   
    
 
       
         
     
                
       
          
                
                    
                
         
        
 
