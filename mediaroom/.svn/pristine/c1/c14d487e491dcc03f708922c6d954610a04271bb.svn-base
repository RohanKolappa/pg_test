package com.ipvs.test;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;
import java.util.Random;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.db.ServiceRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;
public class TestDBQuery {
	
	private Properties props = new Properties();
	private String propsFile = "/com/ipvs/test/xml/dbQuery.properties";
	private ServiceTestUtil mServiceTestUtil = null;
	private String createNodesXml = "";
	private String changeXpath= "";
	//private String parentNode= "";
	private String keyVal = "";
	private String deleteXpath="";
	private int deleteLoopCount=1000;
	private int maxRecords =  1000 ;
	private int repeatTagVal =  5 ;
	private int loopEnd = 250;
	
	private Document doc =null;
	private  int randomseed = 250;
	private String dbObjectName  =null;
	private String requestName  =null;
	private String dataNodeName =null;
	private String serviceName  =null;
	private boolean  createRecords = false;
	
	private static MRLog log = MRLog.getInstance(MRLog.CONSOLE);
	private static int enableLogLevel = MRLog.INFO;
	private static int sumLog = MRLog.ERROR;

   // private static Logger logger = Logger.getLogger("TestDBQuery_sum");
   // private static Logger logger2 = Logger.getLogger("TestDBQuery_detail");
    //private boolean resultReceived;	
    
    public static void addSystemOut(int level, int code, String data, String systemOutAndLog) { 
		try{
			//log.addSystemOut(level, code,data  , systemOutAndLog);
			//if( level <= MRLog.ERROR)
			//	logger.error(data);
			//logger2.log(Level.toLevel(level),  data);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}
	public static void main(String[] args) {
		try {
			addSystemOut(sumLog, MRLog.OK, " TestDBQuery ------------  start   "  , "2");
			TestDBQuery mTestDBQuery = new TestDBQuery ();
			String propsFile2 = getArgument(args, "-file", "").trim();
			boolean  createRecords = Utils.booleanValue( getArgument(args, "-file", "").trim(), false);
			if( !propsFile2.equals("")){
				mTestDBQuery.setPropsFile(propsFile2);
				mTestDBQuery.loadProperties(propsFile2);
			} 
			if(createRecords)
				mTestDBQuery.getProps().setProperty("createRecords", createRecords+"");
 			mTestDBQuery.doProcess();
		} catch (Exception ex) {
			ex.printStackTrace();
		}finally{
			addSystemOut(sumLog, MRLog.OK, " TestDBQuery ------------  End   "  , "2");
		}
		System.exit(0);
	}
  
    public void doProcess()
    {
	    try{
	        createRecords = Utils.booleanValue(props.getProperty("createRecords", ""), createRecords);
	        createNodesXml = props.getProperty("createNodesXml", "/com/ipvs/test/xml/TestDBQuery.xml").trim();
	        changeXpath = props.getProperty("changeXpath", "Groups/Tag").trim();
	       // parentNode = props.getProperty("parentNode", "TestDBQueryRoot").trim();
	        keyVal = props.getProperty("KeyValXpath", "Username").trim();
	        maxRecords = Utils.intValue(props.getProperty("maxRecords", "1000").trim(), 1000);
	        repeatTagVal = Utils.intValue(props.getProperty("repeatTagVal", "5").trim(), 5);
	        loopEnd = maxRecords / repeatTagVal;
	        randomseed = Utils.intValue(props.getProperty("randomseed", "250").trim(), 250);
	        doc = MRXMLUtils.loadXML(createNodesXml, log);
	        dbObjectName = props.getProperty("dbObjectName", "User").trim();
	        requestName = props.getProperty("requestName", "DBTestRequest").trim();
	        dataNodeName = props.getProperty("dataNodeName", "DBTestRequestData").trim();
	        serviceName = props.getProperty("serviceName", "assetadminservice").trim();
	        deleteXpath = props.getProperty("deleteXpath", "//User[Name='dbQuery']").trim();
	        deleteLoopCount = Utils.intValue(props.getProperty("deleteLoopCount", "1000").trim(), 1000);
	        //boolean DummyDatabase = AMStringUtil.booleanValue(props.getProperty("DummyDatabase", ""), false);
	       // mServiceTestUtil.setDummyXMPPImpl(DummyDatabase);
	        if(!login())
	        {
	        	 readRecord();
	            addSystemOut(sumLog, MRLog.OK, " login failed ABORTING   ", "2");
	            return;
	        }
	        try
	        {
	           /* if(createRecords)
	            {
	                delete();
	                create();
	            }*/
	            readRecord();
	        }
	        catch(Exception e)
	        {
	            e.printStackTrace();
	        }
	    }catch(Exception ex)
        {
            ex.printStackTrace();
        }
    	
    }
	/*public static final Random wheel = new Random();
	private String getNextClientData(String seed){
		return  seed +"_"+ System.currentTimeMillis() +"_"+ (Integer.toHexString(wheel.nextInt()));
	}*/
	// ////////////////////////////////////////////////////////////////////////
	public boolean readRecord(){
		try {
			/*read.xml=/com/ipvs/test/xml/dbquery_asset.xml
			loop.nonstop.read=true
			read.loop.count=1
			read.loop.sleep.milisec=1*/			
			String path = props.getProperty("read.xml", "/com/ipvs/test/xml/dbquery_asset.xml").trim();
	        int count= Utils.intValue(props.getProperty("read.loop.count", "1").trim(), 1);
	        boolean isLoop= Utils.booleanValue(props.getProperty("loop.nonstop.read", "false").trim(), false);
	        int isLoopSleep= Utils.intValue(props.getProperty("read.loop.sleep.milisec", "1").trim(), 1);
	        
	        boolean waitforResult= Utils.booleanValue(props.getProperty("read.waitfor.result.synchronized", "true").trim(), true);
	        

			Document doc = MRXMLUtils.loadXMLResource(path, log);
			do{
				for(int i =0;i<count;i++){
					try{
						long startTime =System.currentTimeMillis();
						ServiceTestUtil.RespXML b  =  mServiceTestUtil.doSend( MRXMLUtils.documentToString(doc) , " GetRequest " , waitforResult );
						System.out.println("done " + b.key +"" +b.state +"" +b.xml+ " " + ( System.currentTimeMillis() - startTime));
					} catch (Exception ex) {
						ex.printStackTrace();
						isLoop = false;
					}

				}
				if(isLoopSleep >1)
					Thread.sleep(isLoopSleep);
			}while(isLoop);
	        
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return true;
	}
	public boolean create() {
		try {
			int iCounter = Utils.intValue(props.getProperty("startAddFromNumber", "1").trim(), 1);;
	        int iAddedCounter = 1;
			for(int iRepeat=1; iRepeat < repeatTagVal+1 ; iRepeat++){
				for(int iRec=1;iRec<loopEnd+1 ;iRec++){
					try{
						Element root = (Element)doc.getRootElement().clone();
						List<?> childList = MRXMLUtils.getListXpath(root, changeXpath);
						String value = "";
						for (Iterator<?> iTag = childList.iterator(); iTag.hasNext();) {
							Element fromChildEL = (Element) iTag .next();
							value = MRXMLUtils.getAttributeValue(fromChildEL, "Value", "");
							value = value +"_"+iRec;
							MRXMLUtils.addAttribute(fromChildEL,  "Value", value );
						}
						String userid="";
						if(!keyVal.equals("")){
							Element el = MRXMLUtils.getElementXpath(root, keyVal);
							userid=MRXMLUtils.getValue(el) +"_"+iCounter;
							MRXMLUtils.replaceValueXpath(root, keyVal , userid);
							iCounter++;
							iAddedCounter++;
						}
						sendAddRequest ( root , " Tag value "+ value +"  " + value + " userid  " + userid);
				        if(iAddedCounter > maxRecords)
				            return true;
					} catch (Exception ex) {
						ex.printStackTrace();
					}
	 			}
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return true;
	}	
	public boolean delete() {
		try {
			int iFailed=0;
			int stopDeleteAfterFailures = 3;//deleteLoopCount;
			//for(int iRepeat=1; iRepeat < repeatTagVal ; iRepeat++){
				for(int iRec=1;iRec<deleteLoopCount+1;iRec++){
					try{
		 				Element WhereAssistEL = (Element)doc.getRootElement().clone();
						Element root = (Element)doc.getRootElement().clone();
						boolean b = sendDeleteRequest (root , WhereAssistEL, deleteXpath, iRec) ;
						if(!b)
							iFailed++;
						if(iFailed > stopDeleteAfterFailures){
							addSystemOut(sumLog, MRLog.OK, "Failed 10 times Abort Delete : " , "2");
							break;
						}
					} catch (Exception ex) {
						ex.printStackTrace();
					}
				}
			//}
				//sendDeleteRequest (root , WhereAssistEL,"//User[Username='dbQueryTest5' or Username='dbQueryTest2_1' or Username='dbQueryTest3_2' or Username='dbQueryTest3_0' or Username='dbQueryTest3_1'  or Username='dbQueryTest2_1' or Username='dbQueryTest1_0' or Username='dbQueryTest3_1' or Username='dbQueryTest1_1' or Username='dbQueryTest2_0' ]") ;
 //				StringBuffer WhereAssistXpath=new StringBuffer();  
//				WhereAssistXpath.append("//Tag[ "); 
//				List<?> childList = MRXMLUtils.getListXpath(root, changeXpath);
//				boolean bAppend=false;
//				for (Iterator<?> iTag = childList.iterator(); iTag.hasNext();) {
//					Element fromChildEL = (Element) iTag .next();
//					String name = MRXMLUtils.getAttributeValue(fromChildEL, "Name", "").trim();
//					String value = MRXMLUtils.getAttributeValue(fromChildEL, "Value", "").trim();
//					List<?> attrList = MRXMLUtils.getAttributeList(fromChildEL);
//					value = value +"_"+iRec;
//					if(!bAppend){
//						bAppend=true;
//					} else {
//						WhereAssistXpath.append(" and ");
//					}
//					WhereAssistXpath.append(" ( @Name='"+ name +"' and  @Value='"+ value +"') ");
//				}
//				WhereAssistXpath.append(" ] ");
//				////Tag[  ( @Name='Location' and  @Value='myLocation_1')  and  ( @Name='Domain' and  @Value='myDomain_1')  ] 
//				System.out.println(WhereAssistXpath.toString());
//				sendDeleteRequest (root , WhereAssistEL, WhereAssistXpath.toString());
 			 
			
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return true;
	}	
	

 	public boolean readRecord2(){
		 
		try {
			Random rnd = new Random(randomseed);
			//todo: for some more repeated 
			int iRec =  getRandom(rnd , maxRecords);
			Element WhereAssistEL = (Element)doc.getRootElement().clone();
			Element root = (Element)doc.getRootElement().clone();
			StringBuffer WhereAssistXpath=new StringBuffer(); 
			WhereAssistXpath.append("//Groups   "); 
			List<?> childList = MRXMLUtils.getListXpath(root, changeXpath);
			boolean bAppend=false;
			for (Iterator<?> iTag = childList.iterator(); iTag.hasNext();) {
				Element fromChildEL = (Element) iTag .next();
 				String name = MRXMLUtils.getAttributeValue(fromChildEL, "Name", "").trim();
				String value = MRXMLUtils.getAttributeValue(fromChildEL, "Value", "").trim();
				//List<?> attrList = MRXMLUtils.getAttributeList(fromChildEL);
				value = value +"_"+iRec;
				if(!bAppend){
					bAppend=true;
				} else {
					//WhereAssistXpath.append(" and ");
				}
				WhereAssistXpath.append(" [Tag[@Name='"+ name +"' and  @Value='"+ value +"']] ");
			}
			//WhereAssistXpath.append(" ] ");
			////Tag[  ( @Name='Location' and  @Value='myLocation_1')  and  ( @Name='Domain' and  @Value='myDomain_1')  ] 
			addSystemOut(enableLogLevel, MRLog.OK, "readRecord WhereAssistXpath : " + WhereAssistXpath.toString() , "2");
			int timeout  = mServiceTestUtil.timeout;
			mServiceTestUtil.timeout = 200*60*1000;
			sendGetRequest ( 1 ,repeatTagVal, root , WhereAssistEL, WhereAssistXpath.toString());
			mServiceTestUtil.timeout = timeout;
			
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return true;
	}
	// ////////////////////////////////////////////////////////////////////////

	private boolean sendDeleteRequest ( Element root , Element WhereAssistEL, String WhereAssistXpath, int i){
		boolean b =  false;
		try {
//			String enableFilter="E";
//			String requestAction="DeleteRequest";
//  			
//			ServiceRequest request = getRequest ( root );
//			//action
//			request.getAction().setWhereAssistXMLEL(WhereAssistEL);
//			request.getAction().setWhereXPath(WhereAssistXpath);
//
//			//read
//			request.getAction().getFilter().reset();
//			request.getAction().getFilter().setFilter(enableFilter);
//			request.getAction().setAction(requestAction);
//			
//			Document doc = request.getDoc();
//			 
//			addSystemOut(enableLogLevel, MRLog.OK, " DeleteRequest " +i +" request: "  , "2");
//			addSystemOut(enableLogLevel, MRLog.OK, MRXMLUtils.documentToString(doc) , "2");
//			
//
//			ServiceTestUtil.RespXML resp  =  mServiceTestUtil.doSend(MRXMLUtils.documentToString(doc) , " DeleteRequest " +i);
//			addSystemOut(sumLog, MRLog.OK, "" + i + " "+  resp.key + " Delete "  , "2");
//
//			
//			addSystemOut(enableLogLevel, MRLog.OK, " DeleteRequest " +i +" request: " , "2");
//			addSystemOut(enableLogLevel, MRLog.OK, resp.xml +"" , "2");
			
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return b;
	}
	private boolean sendGetRequest (int startFrom,int countToFetch,  Element root , Element WhereAssistEL, String WhereAssistXpath){
		boolean b =  false;
		try {
			
			//String enableFilter="BE";
			//String requestAction="GetRequest";
  			
			//ServiceRequest request = getRequest ( root );
			/*//action
			request.getAction().setWhereAssistXMLEL(WhereAssistEL);
			request.getAction().setWhereXPath(WhereAssistXpath);
			request.getAction().setStartFrom(startFrom);
			request.getAction().setCountToFetch(countToFetch);
			
			//read
			request.getAction().getFilter().reset();
			request.getAction().getFilter().setFilter(enableFilter);
			request.getAction().setAction(requestAction);
			
			Document doc = request.getDoc();
			addSystemOut(enableLogLevel, MRLog.OK, "sendGetRequest request: "  , "2");
			addSystemOut(enableLogLevel, MRLog.OK, MRXMLUtils.documentToString(doc) , "2");
			
			addSystemOut(sumLog, MRLog.OK, " ///////////////////////////////// Read START  ///////////////////////////////// "  , "2");
			ServiceTestUtil.RespXML resp  =  mServiceTestUtil.doSend( MRXMLUtils.documentToString(doc) , " GetRequest "  );
			addSystemOut(sumLog, MRLog.OK, " ///////////////////////////////// Read END ///////////////////////////////// "  , "2");

			addSystemOut(enableLogLevel, MRLog.OK, "sendGetRequest response: " + resp.key +" state " + resp.state  , "2");
			addSystemOut(sumLog, MRLog.OK, resp.xml +"" , "2");*/

		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return b;
	}
	private boolean sendAddRequest ( Element root , String i){
		boolean b =  false;
		try {
			ServiceRequest request = getRequest ( root );
			String enableFilter="A";
			String requestAction="AddRequest";
			
			request.getAction().getFilter().reset();
			request.getAction().getFilter().setFilter(enableFilter);
			request.getAction().setAction(requestAction);
			
			Document doc = request.getDoc();
			addSystemOut(enableLogLevel, MRLog.OK, "sendAddRequest request: "  , "2");
			addSystemOut(enableLogLevel, MRLog.OK, MRXMLUtils.documentToString(doc) , "2");
			
			ServiceTestUtil.RespXML resp =  mServiceTestUtil.doSend( MRXMLUtils.documentToString(doc) , " AddRequest "  + i);
			
			addSystemOut(sumLog, MRLog.OK, "" + i + " "+  resp.key +" state " + resp.state + " Added "  , "2");
			addSystemOut(enableLogLevel, MRLog.OK, "sendAddRequest response: "  , "2");
			addSystemOut(enableLogLevel, MRLog.OK, resp.xml +"" , "2");
			
		   return b;
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		return b;
	}
	// ////////////////////////////////////////////////////////////////////////
	private ServiceRequest getRequest ( Element root ){
		ServiceRequest request = new ServiceRequest();
		/*request.setHeader(new ServiceRequestHeader());
		request.setAction(new ServiceRequestAction());
		request.setDbObjectName(dbObjectName);
		request.setDataNodeName(dataNodeName);
		
		//header 
		request.getHeader().setClientData("clientData");
		request.getHeader().setLogLevel(0);
		request.getHeader().setRequestName(requestName);
		request.getHeader().setServiceName(serviceName);
		//action
		request.getAction().setRequestID("myTest1");
		request.getAction().setOperationDataELAddUpdate(root);
		request.getAction().setOperationDataELGet(root);
		request.getAction().setOperationDataELDelete(root);*/
		return request;
	}
	// ////////////////////////////////////////////////////////////////////////
//	private String getWhereXpath( int iRec , Element fromChildEL  ){
//		StringBuffer WhereAssistXpath=new StringBuffer(); 
//		try {
//			Random rnd = new Random(randomseed);
//			Element WhereAssistEL = (Element)doc.getRootElement().clone();
//			Element root = (Element)doc.getRootElement().clone();
//			boolean bAppend=false;
//			////Groups  [Tag [ @Name='Domain' and  @Value='dbQueryTest5_1'] ]  [Tag [ @Name='Domain' and  @Value='dbQueryTest5_1'] ]
//			WhereAssistXpath.append("//Groups[Tag[ "); 
//			List<?> childList = MRXMLUtils.getListXpath(root, changeXpath);
//			String name = MRXMLUtils.getAttributeValue(fromChildEL, "Name", "").trim();
//			String value = MRXMLUtils.getAttributeValue(fromChildEL, "Value", "").trim();
//			List<?> attrList = MRXMLUtils.getAttributeList(fromChildEL);
//			value = value +"_"+iRec;
//			if(!bAppend){
//				bAppend=true;
//			} else {
//				WhereAssistXpath.append(" and ");
//			}
//			WhereAssistXpath.append(" ( @Name='"+ name +"' and  @Value='"+ value +"') ");
//			WhereAssistXpath.append(" ] ");
//			
//		} catch (Exception ex) {
//			ex.printStackTrace();
//		}
//		return WhereAssistXpath.toString();
//	}
	private int getPositiveNum(int iRec ){
		if(iRec ==0)
			return 1;
		if(iRec >0)
			return iRec ;
		return iRec * -1;
	}
	private int nextInt(Random rnd ,int iRec ){
 		return getPositiveNum(rnd.nextInt( getPositiveNum( iRec) ));
	}
	private int getRandom(Random rnd , int maxRecords){
		int iRec = nextInt( rnd, maxRecords );
		iRec = nextInt( rnd, iRec );
		while(iRec>maxRecords){
			if(iRec < maxRecords)
				return iRec;
			String strmaxRecords = maxRecords + "";
			String striRec = iRec+"";
			if(striRec.length()> strmaxRecords.length()){
				iRec = Utils.intValue(striRec.substring(0,strmaxRecords.length()+1), iRec);
			}
			iRec = nextInt( rnd, iRec );
		}
		return iRec ;
	}		
	// ////////////////////////////////////////////////////////////////////////
	public TestDBQuery(){
		mServiceTestUtil = new ServiceTestUtil();
		props = new Properties();
		props = loadProperties(props, propsFile);
	}
	public void loadProperties( String propsFile) {
		props = loadProperties(props, propsFile);
	}
	public boolean login(){
		if(!mServiceTestUtil.doLogin_File(props.getProperty("loginXML", ""))){
			addSystemOut(sumLog, MRLog.OK, "FAILED Login: " + mServiceTestUtil.getLoginResponse()  , "2");
 			return false;
		}
		if(!mServiceTestUtil.doSetServiceDomain_File(props.getProperty("setDomainXML", ""))){
			addSystemOut(sumLog, MRLog.OK, "FAILED SetServiceDomain: " + mServiceTestUtil.getLoginResponse()  , "2");
			return false;
		}
		return true;
	}

	public String getPropsFile() {
		return propsFile;
	}
	public void setPropsFile(String propsFile) {
		this.propsFile = propsFile;
	}
	public Properties getProps() {
		return props;
	}
	public void setProps(Properties props) {
		this.props = props;
	}
	//////////////////////////////////////////////////////////////////////////
	public static String getArgument(String[] args, String argumentName, String defaultValue){
		for(int i=0;i<args.length;i++){
			String args_i_1 = args[i];
			if(args_i_1.equalsIgnoreCase(argumentName) ){
				if(args.length > i+1){
					i=i+1;
					String args_i_2 = args[i];
					if(!args_i_2.equals(""))
						return args_i_2;
				}
			}
		}
		return defaultValue;
	}
	/*private static Properties loadProperties2(String propFile){
		  Properties props =new Properties();
		  try
		  {
			  if(propFile.startsWith("/"))
				  props.load(TestDBQuery.class.getResourceAsStream (propFile));
			  else {
				  props.load(new FileInputStream( new File( propFile )));  
			  }
			  
		  }catch(Exception ex){
			  
		  }
		  return props;
	}*/
	private static Properties loadProperties(Properties props , String propFile){
		InputStream in=null;  
		try{
			File file = new File(propFile);
			 if(!file.exists()){
				  in = TestDBQuery.class.getResourceAsStream (propFile);
			 }else {
				  in =  new FileInputStream( new File( propFile ));  
			 }
			 props.load(in);
			  
		  }catch(Exception ex){
			  ex.printStackTrace();
		  }finally{
			  if(in !=null){
				  try{
					  in.close();
				  }catch(Exception ex){}
			  }
			 in=null;
		  }
		  return props;
	}
 
}

 