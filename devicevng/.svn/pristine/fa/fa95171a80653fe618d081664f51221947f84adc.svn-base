package com.barco.device.handler.Media.base;

import org.dom4j.Document;
import org.dom4j.Element;
import org.junit.AfterClass;
import org.junit.BeforeClass;

import com.barco.device.handler.Base.BaseTestSetup;
import com.barco.test.utils.TestConfigConst;
import com.barco.test.utils.TestUtils;
import com.barco.utils.XMLUtils;

/*
 * Source Table Data Example
<SourceTable>
<Instance NID="default.mediastreamsrcportresourcelist.mssprl_57084738-4839-41b1-946f-36aadc24e8de.V2D.SHARED" 
 instanceType="SHARED" timelineID="2b6292aa-6b18-45b4-8577-7ac787e8e874.StreamTrack">
 <Input ssInstanceID="2001" inputType="ENCODER" streamType="V2D" 
  sourceNID="default.mediastreamsrcportresourcelist.mssprl_57084738-4839-41b1-946f-36aadc24e8de" offset="1315" 
  roomNID="" streamNID="" startTime="1363268384956" startTimecode="1363271281714" syncTC="0" timescale="1.0" 
  sourcePortElementID="1" isMulticast="false" 
  NID="default.assetresourcelist.arl_b01aa808-5def-4e78-99f1-da79cc26d429.DeviceStatus.ServiceElementsStatus.StreamingServiceStatus.InstanceList.2001" 
  state="3"/>
  <MediaSource NID="dc06d72c-4256-4f6e-b45a-c61027fe8a37" roomNID="2b6292aa-6b18-45b4-8577-7ac787e8e874" 
   mediaSourceType="ENCODER" startTimecode="1363271281714">
   <Stream NID="683b5d84-6134-40d5-a2a9-ba1b60c52053" 
    destAgentJID="dcuser1@localhost/dcpc1" roomNID="2b6292aa-6b18-45b4-8577-7ac787e8e874" state="8">
    <Output ssInstanceID="2002" outputType="OUTPUT_STREAM" destNID="" streamType="V2D" trickPlay="true" 
     streamURL="v2d://10.51.50.1/port=6060?bandwidth=10240000?avoption=Video?ismulticast=false?enablekbm=false?authscheme=TOKEN?authdata=4f60b616-14e5-4b0e-917e-09e81c005afe" 
     NID="default.assetresourcelist.arl_b01aa808-5def-4e78-99f1-da79cc26d429.DeviceStatus.ServiceElementsStatus.StreamingServiceStatus.InstanceList.2002"/>
	<MediaTimeline id="">
	 <Start WC="1363268384979" Offset="1315" TC="1363271281714"/>
	 <Last WC="1363268384979" Offset="1315" TC="1363271281714" TimeScale="1.0" action="Start" lastState="STARTED_CURRENT" requestedAction="Start" sourceAction="Start" ssTC="" ssWC=""/>
	</MediaTimeline>
   </Stream></MediaSource> 
  </Instance>
</SourceTable>
*/

public class BaseMediaHandlerTestSetup extends BaseTestSetup{
	private static String eventXML=null;
	public static String getEventXML() {
		return eventXML;
	}

	public static void setEventXML(String eventXML) {
		BaseMediaHandlerTestSetup.eventXML = eventXML;
		//System.out.println("EventXMl is: "+eventXML);
	}

	private static String presenceXML;
	public static String getPresenceXML() {
		return presenceXML;
	}

	public static void setPresenceXML(String presenceXML) {
		BaseMediaHandlerTestSetup.presenceXML = presenceXML;
	}

	static Document eventDoc;
	static Document presenceDoc;
	static Element headerElement;
	
	@BeforeClass
	public static void setUpBeforeClass() {
		try {
			TestUtils testUtils = new TestUtils();	
			if(getEventXML()==null){			
							
				setEventXML(testUtils.loadXmlFromResource(TestConfigConst.EVENT));
				//setEventXML(testUtils.readFileFromResource(testUtils.getProperties("/resources/common-test-config.properties", "EventXML")));
				eventDoc = XMLUtils.stringToDocument(eventXML);
			}
			if(getPresenceXML() == null){		
				
				setPresenceXML(testUtils.loadXmlFromResource(TestConfigConst.PRESENCE));
				presenceDoc = XMLUtils.stringToDocument(presenceXML);
			}
			if(!eventXML.equals(null)){
				headerElement = XMLUtils.getElementXpath(eventDoc, "//header");
			}
			BaseTestSetup.setUpBeforeClass();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return;
		}
		
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		System.out.println("In BaseMediaHandlerTestSetup.tearDownAfterClass");
		eventXML=null;
		presenceXML=null;
		eventDoc = null;
		presenceDoc=null;
		headerElement = null;
		BaseTestSetup.tearDownAfterClass();
	}

	
}
