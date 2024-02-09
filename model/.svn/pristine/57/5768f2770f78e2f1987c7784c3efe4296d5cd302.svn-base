Streaming server is a module that caters the sending and receiving of data on TCP/IP for any given devices(I50/XP/etc). To achive this streaming sever objects are created like sender and receiver objects which are independent runing threadsthat take care of transfer of data. 

For example on a transmitter/Tx/Sender Device, a fpga object (receiver) is first created which has frames on a specific queue and a network object (sender) picks these frames from the same queue and sends message on particular listener port on TCP. Basically sending a message is inserting a frame on a listining queue. 
On the receiver/Rx device, It picks up messages on the same listener port from the respective queue. 

To achive the above streaming server api's are used to create and manage streaming server objects. Once an object is created and started it acts as a sender or a receiver object. Basically a object has an id associated to it when created. By creating an object is it not ready for communication, this object has to be started by assigning a queue_id to it. 

Once started a object is ready for communication. In a typical example a fpga receiver object (V2DEncoderPort) is created that would generate the first frame and put it on the queue associated to it. A Sender object is created (V2DecoderPort) that would use the same queie id to get started and these frames are sent on the a particular listener port for any Rx.



Below is the basic/common parameters used to create/start/delete/stop/update an object. For further specific object refere to ssapi.xml to create with additional parameters. 

Add an object
-----------------
action=add id=<object_id> operation=<input/output> srctype/dsttype=<V2DEncoderPort> ioport=<1/2>

action = keyword that signifies the action, add creates an object.
id = ID that represents the object. This is a independent id for each object, any action on this particular object is refered by this id.   
operation = Represents if it is a sender/receiver.
srctype/dsttype = Type of object, an input operation maps to srctype and an output operation maps to dsttype.
ioport = Determines the channel number (default is 1).

Start an object
-----------------
action=start id=<object_id> operation=<input/output> srctype/dsttype=<V2DEncoderPort> queueid=<queueid>

action = keyword that signifies the action, starts an object.
id = ID that represents the object.
operation = Represents if it is a sender/receiver.
srctype/dsttype = Type of object, an input operation maps to srctype and an output operation maps to dsttype.
queueid = Represents the queueid of the object.

Stop an object
------------------
action=stop id=<object_id> operation=<input/output> srctype/dsttype=<V2DEncoderPort> queueid=<queueid>

action = keyword that signifies the action, stops the object with this all connection are disconnected.
id = ID that represents the object.
operation = Represents if it is a sender/receiver.
srctype/dsttype = Type of object, an input operation maps to srctype and an output operation maps to dsttype.
queueid = Represents the queueid of the object.


Delete an object
-------------------
action=delete id=<object_id> operation=<input/output> srctype/dsttype=<V2DEncoderPort>

action = keyword that signifies the action, deletes the object.
id = ID that represents the object.
operation = Represents if it is a sender/receiver.
srctype/dsttype = Type of object, an input operation maps to srctype and an output operation maps to dsttype.

Update an object
-------------------
action=update id=<object_id> operation=<input/output> srctype/dsttype=<V2DEncoderPort> 

action = keyword that signifies the action.
id = ID that represents the object.
operation = Represents if it is a sender/receiver.
srctype/dsttype = Type of object, an input operation maps to srctype and an output operation maps to dsttype.

Notifications
---------------

Streaming Server notifications are generated for actions of an object. 
The notification is in the form of a xml format. The main tag is under SSNotification. The data is displayed under 2 sub tags Header and Data. 

The Header tag consist of the following subtags 
EventType - Specifies the type/status of the event. Below are the list of values            of the type of events.  
            None, Error, Connected, Disconnected, Started, Stopped, EndOfMedia,             Progress, Unknown.
Id        - Id specifies the object id used while creating.
Type      - Specifies if it is a sender/receiver and which type.
Status    - Status code 
WallClock - Time stamp when the notification was created.

and the Data tag has the following subtags
QueueId   - Queue id the object was started on.
Status    - String of status code with respect the header tag.
StartTimeStamp  - Time stamp of the media that is playing.
StartWallClock  - Present time stamp of present object.


Example of the notification snapshot can be found below

<SSNotificationStream>

<SSNotification>

    <Header>
        <EventType>Started</EventType>
        <Id>10000</Id>
        <Type>V2DEncoderPort</Type>
        <Status>3</Status>
        <WallClock>1263431325526</WallClock>
    </Header>
        
    <Data>
        <QueueId>1234</QueueId>
        <Status>Connected Started</Status>
        <StartTimeStamp>1263431325526</StartTimeStamp>
        <StartWallClock>1263431325526</StartWallClock>
    </Data>
    
</SSNotification>



