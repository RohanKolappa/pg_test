The AppServerAgent is responsible for all the MediaRooms
The Sessions are built as a layer on top of MediaRooms
The Session has a pointer to the MediaRoom
The SessionManager registers as an observer on the MediaRoom
The RoomDB calls methods on the observer when presence/state changes