package com.ipvs.test;

import java.util.Hashtable;
import java.util.Properties;
import java.util.Random;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.MRClientAgent;
import com.ipvs.common.MRLog;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class DBApiTest {
	private Properties props = null;
	//private boolean waitforResult=true;
	private String loginXML="";
	private String setDomainXML="";
	private String objectType="";
	private MRLog log= null;
	
	//private Hashtable<String, Document> xmls = new Hashtable<String, Document>();
	private ServiceTestUtil mServiceTestUtil = null;
	private String objectNID2="default.serviceresourcelist.srl_970ad6b2-6e5e-4892-9fb8-2be68a37cc2f";
	private static String propsFile = "/com/ipvs/test/xml/DBApiTest.properties";
	public static void main(String[] args) {
		try {
			String propsPath = propsFile;
			String loadType = "file";//todo
			if (args.length > 1) {
				//loadType = args[0].trim();
				propsPath = args[0].trim();
			}
			DBApiTest test = new DBApiTest();
			test.initProperties(loadType, propsPath);
			test.process();
			System.out.println(  "<!-- ");
			
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		System.out.println(  " --> ");
		System.exit(0);
		
	}

	public void process() {
		try {
			mServiceTestUtil = new ServiceTestUtil();
			//Login
			//setDomaine
			boolean delete=false;
			if (login()) {
				System.out.println(  " --> \n <!-- read start -->\n<!--");
				processRead();
				System.out.println(  " --> \n <!-- read end -->\n<!--");
				if(!delete){
					return;
				}				
				
				String nodeId2= processAdd();
				System.out.println(  " --> \n <!-- Add start -->\n<!-- ");
				String nodeId= processAdd();	
				System.out.println(  " --> \n <!-- Add end-->\n<!--");
				
				System.out.println(  " --> \n <!-- Replace start -->\n<!--");
				processReplace(nodeId, false);
				System.out.println(  " --> \n <!-- Replace end -->\n<!--");
				
				System.out.println(  " --> \n <!-- read start -->\n<!--");
				processRead();
				System.out.println(  " --> \n <!-- read end -->\n<!--");
				if(!delete){
					return;
				}
				
				System.out.println(  " --> \n <!-- ReadQuery start -->\n<!--");
				processReadQuery();
				System.out.println(  " --> \n <!-- ReadQuery end -->\n<!--");				
				
				String nodeId3= processAdd();
				processReplace(nodeId3, false);
				
				
				System.out.println(  " --> \n <!-- processDelete start -->\n<!--");
				processDelete(nodeId3, false);
				System.out.println(  " --> \n <!-- processDelete end -->\n<!--");

				System.out.println(  " --> \n <!-- Replace documents start -->\n<!--");
				processReplace(nodeId, true);
				System.out.println(  " --> \n <!--  Replace documents end-->\n<!--");
				
				System.out.println(  " --> \n <!-- Delete documents start -->\n<!--");
				processDelete(nodeId, true);
				System.out.println(  " --> \n <!-- Delete documents start -->\n<!--");
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		//Logout
		logout() ;
		
	}

	////////////////////////ReadQuery  ////////////////////////////////////////////////////////////////////////
	////////////////////////ReadQuery  ////////////////////////////////////////////////////////////////////////
	////////////////////////ReadQuery  ////////////////////////////////////////////////////////////////////////
	/*
	  -read query 	
		-single table query
		-two table query
		-three table query		
	 
	 */
	public String  processReadQuery(){
		String returnNID="";
		try {
			int testCase=1;
			String readQuery ="";
			Document doc = null;
			String objectType1="AssetResource";
			String objectType2="ServiceResource";
			String objectType3="GroupTagDef";
			
			String objectNID1="default.assetresourcelist.managementserver";
			
			
			String objectListNID1="default.assetresourcelist";
			String objectListNID2="default.serviceresourcelist";
			String objectListNID3="default.grouptagdeflist";
			////////////////////////////////////// add document ////////////////////////////////////////////////////
			testCase=1;
			readQuery = getReadQuery(testCase,objectListNID1, objectListNID2, "/@NID", "/@NID");;
			doc = readQuery(testCase, readQuery);

			testCase=2;
			readQuery =  getReadQuery(testCase,objectType1, objectType2, "/@NID", "/@NID");
			doc = readQuery(testCase, readQuery);	

			testCase=3;
			readQuery =  getReadQuery(testCase,objectType1, objectType3, "/@NID", "/@NID");
			doc = readQuery(testCase, readQuery);				

			testCase=4;
			readQuery =  getReadQuery(testCase,objectType1, objectType2, "", "");
			doc = readQuery(testCase, readQuery);			
			
			testCase=5;
			readQuery =  getReadQuery(testCase,objectType1, objectType3, "", "");
			doc = readQuery(testCase, readQuery);			
			
			///////////////////
			testCase=6;
			readQuery =  getReadQuery(testCase,objectListNID1, objectNID2, "/Info", "/Contact");
			doc = readQuery(testCase, readQuery);					

			testCase=7;
			readQuery =  getReadQuery(testCase,objectType1, objectNID2, "/Info", "/Contact");
			doc = readQuery(testCase, readQuery);	

			testCase=8;
			readQuery =  getReadQuery(testCase,objectType1, objectNID2, "/Info", "/Contact");
			doc = readQuery(testCase, readQuery);				

			testCase=9;
			readQuery =  getReadQuery(testCase,objectType1, objectNID2, "", "");
			doc = readQuery(testCase, readQuery);		
			
			testCase=10;
			readQuery =  getReadQuery(testCase,objectType1, "default.grouptagdeflist", "", "");
			doc = readQuery(testCase, readQuery);		
			
			///////////////////
			testCase=11;
			readQuery =  getReadQuery(testCase,objectNID1, objectNID2, "/Info", "/Contact");
			doc = readQuery(testCase, readQuery);					

			testCase=12;
			readQuery =  getReadQuery(testCase,objectNID1, objectNID2, "/Info", "/Contact");
			doc = readQuery(testCase, readQuery);	

			testCase=13;
			readQuery =  getReadQuery(testCase,objectNID1, objectNID2, "/Info", "/Contact");
			doc = readQuery(testCase, readQuery);				

			testCase=14;
			readQuery =  getReadQuery(testCase,objectNID1, objectNID2, "", "");
			doc = readQuery(testCase, readQuery);	
			
			testCase=15;
			readQuery =  getReadQuery(testCase,objectNID1,objectListNID3, "", "");
			doc = readQuery(testCase, readQuery);	
			
		} catch (Exception e) {
			e.printStackTrace();
		}
		return returnNID;
	}	
			
	private String getReadQuery( int testCase , String NID1, String NID2, String child1 , String child2){
		 StringBuffer buf = new StringBuffer();
		 try{
			 switch(testCase){
			 case 1:
			 	buf.append("for $b in  index-scan('IPVSis_colnids_default', '"+NID1+"', 'EQ')/.."+child1+" return string($b)");
			 	break;
			 case 2:
				 	buf.append("for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return string($b)");
				 	break;
			 case 3:
				 	buf.append("let $list1:= for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return string($b) ");
				 	buf.append("let $list2:= for $c in #OBJ_"+NID2+"_OBJ#"+child1+" return string($c) ");
				 	buf.append("return <result>  <list"+testCase+"a>{$list1}</list"+testCase+"a> <list"+testCase+"b>{$list2}</list"+testCase+"b></result>");
				 	break;
			 case 4:
				 	buf.append("let $list1:= for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return $b ");
				 	buf.append("let $list2:= for $c in #OBJ_"+NID2+"_OBJ#"+child1+" return $c ");
				 	buf.append("return <result>  <list"+testCase+"a>{$list1}</list"+testCase+"a> <list"+testCase+"b>{$list2}</list"+testCase+"b></result>");
				 	break;
			 case 5:
				 	buf.append("let $list1:= for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return $b ");
				 	buf.append("let $list2:= for $b in #OBJ_"+NID2+"_OBJ#"+child1+" return $b ");
				 	buf.append("return <result>  <list"+testCase+"a>{$list1}</list"+testCase+"a> <list"+testCase+"b>{$list2}</list"+testCase+"b></result>");
				 	break;					 	
			 //using NID	 	amd returnn is child xpath
			 case 6:
				 	buf.append("for $b in  index-scan('IPVSi_nids_default', '"+NID1+"', 'EQ')/.."+child1+" return string($b)");
				 	break;
			 case 7:
				 	buf.append("for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return $b");
				 	break;
			 case 8:
				 	buf.append("let $list1:= for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return $b ");
				 	buf.append("let $list2:= for $b in #OBJ_"+NID2+"_OBJ#"+child2+" return $b ");
				 	buf.append("return <result>  <list"+testCase+"a>{$list1}</list"+testCase+"a> <list"+testCase+"b>{$list2}</list"+testCase+"b></result>");
				 	break;
			 case 9:
				 	buf.append("let $list1:= for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return $b ");
				 	buf.append("let $list2:= for $b in #OBJ_"+NID2+"_OBJ#"+child2+" return $b ");
				 	buf.append("return <result>  <list"+testCase+"a>{$list1}</list"+testCase+"a> <list"+testCase+"b>{$list2}</list"+testCase+"b></result>");
				 	break;				 
			case 10:
				 	buf.append("let $list1:= for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return $b ");
				 	buf.append("let $list2:= for $b in #OBJ_"+NID2+"_OBJ#"+child2+" return $b ");
				 	buf.append("return <result>  <list"+testCase+"a>{$list1}</list"+testCase+"a> <list"+testCase+"b>{$list2}</list"+testCase+"b></result>");
				 	break;	
			///
			case 11:
				 	buf.append("for $b in  index-scan('IPVSi_nids_default', '"+NID1+"', 'EQ')/.."+child1+" return string($b)");
				 	break;
			 case 12:
				 	buf.append("for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return $b");
				 	break;
			 case 13:
				 	buf.append("let $list1:= for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return $b ");
				 	buf.append("let $list2:= for $b in #OBJ_"+NID2+"_OBJ#"+child2+" return $b ");
				 	buf.append("return <result>  <list"+testCase+"a>{$list1}</list"+testCase+"a> <list"+testCase+"b>{$list2}</list"+testCase+"b></result>");
				 	break;
			 case 14:
				 	buf.append("let $list1:= for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return $b ");
				 	buf.append("let $list2:= for $b in #OBJ_"+NID2+"_OBJ#"+child2+" return $b ");
				 	buf.append("return <result>  <list"+testCase+"a>{$list1}</list"+testCase+"a> <list"+testCase+"b>{$list2}</list"+testCase+"b></result>");
				 	break;				 
			case 15:
				 	buf.append("let $list1:= for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return $b ");
				 	buf.append("let $list2:= for $b in #OBJ_"+NID2+"_OBJ#"+child2+" return $b ");
				 	buf.append("return <result>  <list"+testCase+"a>{$list1}</list"+testCase+"a> <list"+testCase+"b>{$list2}</list"+testCase+"b></result>");
				 	break;
				 	
			 				 	
			 default:
				 	buf.append("for $b in #OBJ_"+NID1+"_OBJ#"+child1+" return string($b)");
				 break;
			 }
		 }catch(Exception ex){
			 ex.printStackTrace();
		 }
		 return buf.toString();
	}
	public Document readQuery ( int testCase, String readQuery) {
		Document doc = null;
		try{
			String ID=objectType+"_readQuery_"+testCase;
			String xmlFilePath=objectType+"_BASE_ALL_READ_QUERY.xml";
			doc = AssetAdminServiceUtils.getAssetAdminRequest(log, xmlFilePath);
			MRXMLUtils.setValueXpath(doc, "//ReadQueryRequest/Query", readQuery);
			doc = sendRequest(ID, doc);
			
		}catch(Exception ex){
			ex.printStackTrace();
		}
		if(doc !=null){
			String readQueryResponse="";
			try {
				readQueryResponse = MRXMLUtils.getValueXpath(doc, "//ReadQueryResponse/ResponseXML/ReadQueryResult", "");
			} catch (Exception e) {
				e.printStackTrace();
			}
			////System.out.println("///////readQueryResponse/////// "  + testCase );
			////System.out.println(readQuery);
			////System.out.println("//////////////////////////////////////////"+ testCase);
			////System.out.println(readQueryResponse);
			////System.out.println("///////readQueryResponse/////// " + testCase);
		}
		return doc ;
	}
	////////////////////////ReadQuery  ////////////////////////////////////////////////////////////////////////
	////////////////////////ReadQuery  ////////////////////////////////////////////////////////////////////////
	////////////////////////ReadQuery  ////////////////////////////////////////////////////////////////////////

	 
	/*
	  
		-list
		  -read single
		  	-  root + where clause
		  	-  root + where clause + sort
		  	-  root + where clause + sort + select filter
		  	-  childxPath + where clause
		  	-  childxPath + where clause + sort
		  	-  childxPath + where clause + sort + select filter
		  -read List
		  	-  root for all documents
		  	-  root + where clause
		  	-  root + where clause + sort
		  	-  root + where clause + sort + select filter
		  	-  childxPath for all documents
		  	-  childxPath + where clause
		  	-  childxPath + where clause + sort
		  	-  childxPath + where clause + sort + select filter
		-document
		  -read single
		  	-  root + where clause
		  	-  root + where clause + sort
		  	-  root + where clause + sort + select filter
		  	-  childxPath + where clause
		  	-  childxPath + where clause + sort
		  	-  childxPath + where clause + sort + select filter
		  -read List
		  	-  root for all documents
		  	-  root + where clause
		  	-  root + where clause + sort
		  	-  root + where clause + sort + select filter
		  	-  childxPath for all documents
		  	-  childxPath + where clause
		  	-  childxPath + where clause + sort
		  	-  childxPath + where clause + sort + select filter

	 		
	 */
	
	public String  processRead(){
		String returnNID="";
		try {
			int testCase=1;
			String objectType=this.objectType;
			String NID=""; 
			String childXpath=""; 
			String[]  whereXpath={"","",""};
			String[]  whereSort={"","",""};
			String selectXml="";
			boolean sortAsending=true;
			Hashtable<String,String> replaceList = new Hashtable<String, String>();
			Document doc =null;
			 
			String objectType1="AssetResource";
			String objectType2="ServiceResource";
			String objectType3="GroupTagDef";
			
			String objectNID1="default.assetresourcelist.managementserver";
			
			String objectListNID1="default.assetresourcelist";
			String objectListNID2="default.serviceresourcelist";
			String objectListNID3="default.grouptagdeflist";			
			////////////////////////////////////// add document ////////////////////////////////////////////////////
			//selectXml - one node
			testCase=1;
			whereXpath[0] = "//Info[Type='MS-Device']";
			whereXpath[1] = "//Info[Title='managementserver']";
			whereSort[0] = "Info/Title";
			whereSort[1] = "Info/Type";
			whereSort[2] = "@NID";
			childXpath="";
			selectXml="<SelectXML><xpath>//Info</xpath></SelectXML>";
			doc = read(testCase, objectType1, "", childXpath, whereXpath, whereSort, sortAsending, selectXml, replaceList);
						
			//selectXml - two nodes
			testCase=2;
			whereXpath[0] = "//addDOCc33[.='addDOCc33']";
			whereXpath[1] = "addLISTrwc1[replaceLISTc3='replaceDoc3']";
			whereSort[0] = "addLISTrwc1/replaceLISTr2/newaddLISTc4";
			whereSort[1] = "addDOCrwc1/replaceDOCc3";
			whereSort[2]="";
			childXpath="";
			selectXml="<SelectXML><abc> <xpath>replaceLISTr2/newaddLISTc4</xpath>  <xpath>//addLISTc33 </xpath></abc></SelectXML>";
			doc = read(testCase, objectType, NID, childXpath, whereXpath, whereSort, sortAsending, selectXml, replaceList);
 			
			
			//selectXQuery
			testCase=3;
			whereXpath[0] = "//addDOCc33[.='addDOCc33']";
			whereXpath[1] = "addLISTrwc1[replaceLISTc3='replaceDoc3']";
			whereSort[0] = "addLISTrwc1/replaceLISTr2/newaddLISTc4";
			whereSort[1] = "addDOCrwc1/replaceDOCc3";
			whereSort[2]="";
			childXpath="";
			selectXml="<SelectXQuery>count($VAR/replaceLISTr2/newaddLISTc4)</SelectXQuery>";
			doc = read(testCase, objectType, NID, childXpath, whereXpath, whereSort, sortAsending, selectXml, replaceList);

			//selectXQuery
			testCase=4;
			whereXpath[0] = "//addDOCc33[.='addDOCc33']";
			whereXpath[1] = "addLISTrwc1[replaceLISTc3='replaceDoc3']";
			whereSort[0] = "addLISTrwc1/replaceLISTr2/newaddLISTc4";
			whereSort[1] = "addDOCrwc1/replaceDOCc3";
			whereSort[2]="";
			childXpath="";
			selectXml="<SelectXQuery><aa> {$VAR/replaceLISTr2/newaddLISTc4 } <count>{count($VAR/replaceLISTr2/newaddLISTc4)}</count></aa></SelectXQuery>";
			doc = read(testCase, objectType, NID, childXpath, whereXpath, whereSort, sortAsending, selectXml, replaceList);
			
			//selectXQuery
			testCase=5;
			
			whereXpath[0] = "//Info[Type='AppService']";
			whereXpath[1] = "//Info[Description='App Service']";
			whereSort[0] = "Info/Title";
			whereSort[1] = "//Description";
			whereSort[2] = "@NID";
			childXpath="";
			selectXml="<SelectXQuery>  <abc>{$VAR/Info/Title}  {$VAR//Description}  {count($VAR//Tag)} </abc></SelectXQuery>";
			doc = read(testCase, objectType2, "", childXpath, whereXpath, whereSort, sortAsending, selectXml, replaceList);
			
			
			
			//childXpath
			testCase=6;
			whereXpath[0] = "//Info[Type='AppService']";
			whereXpath[1] = "//Info[Description='App Service']";
			whereSort[0] = "Info/Title";
			whereSort[1] = "//Description";
			whereSort[2] = "@NID";
			selectXml="";
			childXpath="Info";
			doc = read(testCase, objectType2, "", childXpath, whereXpath, whereSort, sortAsending, selectXml, replaceList);
						
			//childXpath
			testCase=7;
			whereXpath[0] = "//Info[Type='AppService']";
			whereXpath[1] = "//Info[Description='App Service']";
			whereSort[0] = "Info/Type";
			whereSort[1] = "//Description";
			whereSort[2] = "@NID";
			selectXml="";
			childXpath="//Type";
			doc = read(testCase, objectType2, "", childXpath, whereXpath, whereSort, sortAsending, selectXml, replaceList);

			testCase=8;
			whereXpath[0] = "";
			whereXpath[1] = "";
			whereSort[0] = "addLISTc33";
			whereSort[1] = "../@NID";
			whereSort[2]="";
			selectXml="";
			childXpath="addLISTrwc111";
			doc = read(testCase, objectType, NID, childXpath, whereXpath, whereSort, sortAsending, selectXml, replaceList);
			
			testCase=9;
			whereXpath[0] = "";
			whereXpath[1] = "";			
			whereSort[0] = "Info/Title";
			whereSort[1] = "@NID";
			whereSort[2]="";
			childXpath="//Info";			
			selectXml="<SelectXQuery><abc><xpath>../contact</xpath> <xpath>Title</xpath></abc></SelectXQuery>";
			childXpath="Info";
			doc = read(testCase, objectType1, NID, childXpath, whereXpath, whereSort, sortAsending, selectXml, replaceList);
			 
		} catch (Exception e) {
			e.printStackTrace();
		}
		return returnNID;
	}	
	
	public Document read ( int testCase, String objectType, String NID, String childXpath, String[]  whereXpath, String[]  whereSort, boolean sortAsending, String selectXml,  Hashtable<String,String> replaceList ) {
		Document doc = null;
		try{
			String ID=objectType+"_GET_"+testCase;
			String xmlFilePath=objectType+"_BASE_ALL_GET.xml";
			doc = AssetAdminServiceUtils.getAssetAdminRequest(log, xmlFilePath);
			 
			//replace if any values from hashtable
			for (String xpath : replaceList.keySet()) {
				String value = replaceList.get(xpath);
				MRXMLUtils.setValueXpath(doc, xpath, value);
			}	
			
			Element parent =null;
			MRXMLUtils.setValueXpath(doc, "//GetRequest/PathSpec/In/ChildXPath", childXpath);
			
			
			for (int i = 0; i < whereSort.length; i++) {
				if(i==0){
					MRXMLUtils.setValueXpath(doc, "//GetRequest/PathSpec/Where/WhereXPath", whereXpath[0]);
				}else{
					parent = MRXMLUtils.getElementXpath(doc, "//GetRequest/PathSpec/Where");
					MRXMLUtils.addElement(parent, "WhereXPath", whereXpath[i]);
				}
			}

			for (int i = 0; i < whereSort.length; i++) {
				if(i==0){
					MRXMLUtils.setValueXpath(doc, "//GetRequest/PathSpec/Select/ListSpec/SortByXPathList/SortByXpath", whereSort[0]);
				}else{
					parent = MRXMLUtils.getElementXpath(doc, "//GetRequest/PathSpec/Select/ListSpec/SortByXPathList");
					MRXMLUtils.addElement(parent, "SortByXpath", whereSort[i]);
				}
			}
			
			//selectXml could be xml or xpath 
			try{
				if(!selectXml.equals("")){
					Element el   = MRXMLUtils.getElementXpath(doc, "//GetRequest/PathSpec/Select/ItemSpec/SelectXML");
					parent = MRXMLUtils.getParentElement(el);
					el.detach();
					Document doc2= MRXMLUtils.stringToDocument(selectXml);
					MRXMLUtils.addElement(parent, (Element)doc2.getRootElement().detach());
				}
			}catch(Exception ex){
				MRXMLUtils.setValueXpath(doc, "//GetRequest/PathSpec/Select/ItemSpec/SelectXML", selectXml);
				ex.printStackTrace();
			}
			 
			doc = sendRequest(ID, doc);
			
		}catch(Exception ex){
			ex.printStackTrace();
		}
		String readQueryResponse="";
		try {
			Element result = MRXMLUtils.getFirstChildXpath(doc.getRootElement(), "//ResponseXML");
			if(result !=null){
				readQueryResponse = MRXMLUtils.elementToString(result);
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		}
		////System.out.println("///////readResponse/////// "  + testCase );
		////System.out.println(readQueryResponse);
		////System.out.println("///////readQueryResponse/////// " + testCase);
		return doc ;
	}
	 
	
	 
	////////////////////////ADD  ////////////////////////////////////////////////////////////////////////
	////////////////////////ADD  ////////////////////////////////////////////////////////////////////////
	////////////////////////ADD  ////////////////////////////////////////////////////////////////////////
	/*
	 1- using list  
	 	-add document
	 	-add child
		 	-to root + all docs in list 
		 		-to root + with docs for where clause in list
		 	-to childXpath + all docs in list 
		 		-to childXpath + with docs for where clause in list
	 	
	 2- using docNID
	 	-add document
	 	-add child
		 	-to root + all docs in list 
		 		-to root + with docs for where clause in list
		 	-to childXpath + all docs in list 
		 		-to childXpath + with docs for where clause in list
	 	
	 */
	
	public String  processAdd(){
		String returnNID="";
		try {
			int testCase=1;
			String objectType=this.objectType;
			String NID="";
			String rootChildXpath=".";
			String childXpath="";
			String whereXpath="";
			String childXml="";
			Hashtable<String,String> replaceList = new Hashtable<String, String>();
			/*
			 1- using list  
			 	-add document
			 	-add child
				 	-to root + all docs in list 
				 		-to root + with docs for where clause in list
				 	-to childXpath + all docs in list 
				 		-to childXpath + with docs for where clause in list
			 	
			 2- using docNID
			 	-add child
				 	-to root + all docs in list 
				 		-to root + with docs for where clause in list
				 	-to childXpath + all docs in list 
				 		-to childXpath + with docs for where clause in list
			 	
			 */
			////////////////////////////////////// add document ////////////////////////////////////////////////////
			testCase=1;
			Document doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			//System.out.println("doc " + MRXMLUtils.documentToString(doc));
			Element respData= MRXMLUtils.getElementXpath(doc, "//ResponseXML/"+objectType);
			returnNID= MRXMLUtils.getAttributeValue(respData, "NID", "");
 

			/////////////////////// using list - add child   ///////////////////////////////////////
			//to root + all docs in list 
			testCase=2;
			childXml="<addLISTrwc1>addLISTrwc1</addLISTrwc1>";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to root + with docs for where clause in list
			testCase=3;
			childXml="<addLISTr2>addLISTr2</addLISTr2>";
			whereXpath="//"+objectType+"[addLISTrwc1='addLISTrwc1']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);


			//to childXpath + all docs in list 
			testCase=4;
			childXml="<addLISTc3>addLISTc3</addLISTc3>";
			childXpath="addLISTrwc1";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to childXpath + with docs for where clause in list
			testCase=5;
			childXml="<addLISTc4>addLISTc4</addLISTc4>";
			childXpath="addLISTrwc1";
			whereXpath="//"+objectType+"[addLISTrwc1='addLISTrwc1']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			
			
			
			/////////////////////// using list - add child   ///////////////////////////////////////
			NID=returnNID;
			//to root + all docs in list 
			testCase=6;
			childXml="<addDOCrwc1>addDOCrwc1</addDOCrwc1>";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to root + with docs for where clause in list
			testCase=7;
			childXml="<addDOCr2>addDOCr2</addDOCr2>";
			whereXpath="//"+objectType+"[addDOCrwc1='addDOCrwc1']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);


			//to childXpath + all docs in list 
			testCase=8;
			childXml="<addDOCc3>addDOCc3</addDOCc3>";
			childXpath="addDOCrwc1";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to childXpath + with docs for where clause in list
			testCase=9;
			childXml="<addDOCc4>addDOCc4</addDOCc4>";
			childXpath="addDOCrwc1";
			whereXpath="//"+objectType+"[addDOCrwc1='addDOCrwc1']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);

			

			////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////// using list - add child   ///////////////////////////////////////
			//to root + all docs in list 
			testCase=2;
			childXml="<addLISTrwc11>addLISTrwc11</addLISTrwc11>";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to root + with docs for where clause in list
			testCase=3;
			childXml="<addLISTr22>addLISTr22</addLISTr22>";
			whereXpath="//"+objectType+"[addLISTrwc11='addLISTrwc11']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);


			//to childXpath + all docs in list 
			testCase=4;
			childXml="<addLISTc33>addLISTc33</addLISTc33>";
			childXpath="addLISTrwc11";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to childXpath + with docs for where clause in list
			testCase=5;
			childXml="<addLISTc44>addLISTc44</addLISTc44>";
			childXpath="addLISTrwc11";
			whereXpath="//"+objectType+"[addLISTrwc11='addLISTrwc11']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			
			
			
			/////////////////////// using list - add child   ///////////////////////////////////////
			NID=returnNID;
			//to root + all docs in list 
			testCase=6;
			childXml="<addDOCrwc11>addDOCrwc11</addDOCrwc11>";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to root + with docs for where clause in list
			testCase=7;
			childXml="<addDOCr22>addDOCr22</addDOCr22>";
			whereXpath="//"+objectType+"[addDOCrwc11='addDOCrwc11']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);


			//to childXpath + all docs in list 
			testCase=8;
			childXml="<addDOCc33>addDOCc33</addDOCc33>";
			childXpath="addDOCrwc11";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to childXpath + with docs for where clause in list
			testCase=9;
			childXml="<addDOCc44>addDOCc44</addDOCc44>";
			childXpath="addDOCrwc11";
			whereXpath="//"+objectType+"[addDOCrwc11='addDOCrwc11']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);

			
			
			
			////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////// using list - add child   ///////////////////////////////////////
			//to root + all docs in list 
			testCase=2;
			childXml="<addLISTrwc111>addLISTrwc111</addLISTrwc111>";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to root + with docs for where clause in list
			testCase=3;
			childXml="<addLISTr222>addLISTr222</addLISTr222>";
			whereXpath="//"+objectType+"[addLISTrwc111='addLISTrwc111']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);


			//to childXpath + all docs in list 
			testCase=4;
			childXml="<addLISTc333>addLISTc333</addLISTc333>";
			childXpath="addLISTrwc111";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to childXpath + with docs for where clause in list
			testCase=5;
			childXml="<addLISTc444>addLISTc444</addLISTc444>";
			childXpath="addLISTrwc111";
			whereXpath="//"+objectType+"[addLISTrwc111='addLISTrwc111']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			
			
			
			/////////////////////// using list - add child   ///////////////////////////////////////
			NID=returnNID;
			//to root + all docs in list 
			testCase=6;
			childXml="<addDOCrwc111>addDOCrwc111</addDOCrwc111>";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to root + with docs for where clause in list
			testCase=7;
			childXml="<addDOCr222>addDOCr222</addDOCr222>";
			whereXpath="//"+objectType+"[addDOCrwc111='addDOCrwc111']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);


			//to childXpath + all docs in list 
			testCase=8;
			childXml="<addDOCc333>addDOCc333</addDOCc333>";
			childXpath="addDOCrwc111";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
			 
			
			//to childXpath + with docs for where clause in list
			testCase=9;
			childXml="<addDOCc444>addDOCc444</addDOCc444>";
			childXpath="addDOCrwc111";
			whereXpath="//"+objectType+"[addDOCrwc111='addDOCrwc111']";
			doc = add(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);

			
			
		} catch (Exception e) {
			e.printStackTrace();
		}
		return returnNID;
	}	
	public Document add ( int testCase, String objectType, String NID, String rootChildXpath, String childXpath, String  whereXpath, String childXml , Hashtable<String,String> replaceList ) {
		Document doc = null;
		try{

			String ID=objectType+"_add_"+testCase;
			String xmlFilePath=objectType+"_BASE_ALL_ADD.xml";;
			doc = AssetAdminServiceUtils.getAssetAdminRequest(log, xmlFilePath);
			
			if(testCase >1) {
				//replace operationData
				Element operationDataReq = MRXMLUtils.stringToElement(childXml);
				Element operationData = MRXMLUtils.getElementXpath(doc, "//OperationData");
				Element parent = MRXMLUtils.getParentElement(operationData);
				operationData.detach();
				parent = MRXMLUtils.addElement(parent, "OperationData");
				MRXMLUtils.addElement(parent, operationDataReq);
				
			}
			
			//replace if any values from hashtable
			for (String xpath : replaceList.keySet()) {
				String value = replaceList.get(xpath);
				MRXMLUtils.setValueXpath(doc, xpath, value);
			}	
			
			switch(testCase){
				//////////////////// list//////////////////////////// 
				case 1: //documents
					//MRXMLUtils.setValueXpath(doc, "//AddRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					break;
				//////////////////// list//////////////////////////// 
				case 2: //parent-as-root  
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/In/ChildXPath", rootChildXpath);
					break;
				case 3:  //parent-as-root + where
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/In/ChildXPath", rootChildXpath);
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/Where/WhereXPath", whereXpath);
					break;	
				case 4: //parent-as-xpath   
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/In/ChildXPath", childXpath);
					break;
				case 5:  //parent-as-xpath + where
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/In/ChildXPath", childXpath);
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/Where/WhereXPath", whereXpath);
					break;	
					
				//////////////////// docNID//////////////////////////// 
				//case 7: //documents
				//	//MRXMLUtils.setValueXpath(doc, "//AddRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
				//	break;
				//////////////////// list//////////////////////////// 
				case 6: //parent-as-root 
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentNID",  NID);
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/In/ChildXPath", rootChildXpath);
					break;
				case 7:  //parent-as-root + where
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentNID", NID);
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/In/ChildXPath", rootChildXpath);
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/Where/WhereXPath", whereXpath);
					break;	
				case 8: //parent-as-xpath
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentNID", NID);
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/In/ChildXPath", childXpath);
					break;
				case 9:  //parent-as-xpath + where
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentNID", NID);
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/In/ChildXPath", childXpath);
					MRXMLUtils.setValueXpath(doc, "//AddRequest/ParentPathSpec/Where/WhereXPath", whereXpath);
					break;	
				//////////////////// add to parent xpath//////////////////////////// 
	 			default:
					break;
			}
			doc = sendRequest(ID, doc);
			
		}catch(Exception ex){
			ex.printStackTrace();
		}
		return doc ;
	}
	////////////////////////ADD  ////////////////////////////////////////////////////////////////////////
	////////////////////////ADD  ////////////////////////////////////////////////////////////////////////
	////////////////////////ADD  ////////////////////////////////////////////////////////////////////////

	
	

	////////////////////////REPLACE  ////////////////////////////////////////////////////////////////////////
	////////////////////////REPLACE  ////////////////////////////////////////////////////////////////////////
	////////////////////////REPLACE  ////////////////////////////////////////////////////////////////////////
	
	/*
	 1- using list  
	 	-replace documents
	 	-replace child
		 	-to root + all docs in list 
		 		-to root + with docs for where clause in list
		 	-to childXpath + all docs in list 
		 		-to childXpath + with docs for where clause in list
	 	
	 2- using docNID
	 	-replace document
	 	-replace child
		 	-to root + all docs in list 
		 		-to root + with docs for where clause in list
		 	-to childXpath + all docs in list 
		 		-to childXpath + with docs for where clause in list
	 	
	 	
	 */
	/*
	 <addLISTrwc1><addLISTc3>addLISTc3</addLISTc3>addLISTrwc1</addLISTrwc1>
	 <addLISTr2><addLISTc4>addLISTc4</addLISTc4>addLISTr2</addLISTr2>
	 <addDOCrwc1><addDOCc3>addDOCc3</addDOCc3>addDOCrwc1</addDOCrwc1>
	 <addDOCr2><addDOCc4>addDOCc4</addDOCc4>addDOCr2</addDOCr2>
	 <addLISTrwc11><addLISTc33>addLISTc33</addLISTc33>addLISTrwc11</addLISTrwc11>
	 <addLISTr22><addLISTc44>addLISTc44</addLISTc44>addLISTr22</addLISTr22>
	 <addDOCrwc11><addDOCc33>addDOCc33</addDOCc33>addDOCrwc11</addDOCrwc11>
	 <addDOCr22><addDOCc44>addDOCc44</addDOCc44>addDOCr22</addDOCr22>
	 <addLISTrwc111>
	  <addLISTc333>addLISTc333</addLISTc333>addLISTrwc111</addLISTrwc111>
	 <addLISTr222>
	  <addLISTc444>addLISTc444</addLISTc444>addLISTr222</addLISTr222>
	 <addDOCrwc111>
	  <addDOCc333>addDOCc333</addDOCc333>addDOCrwc111</addDOCrwc111>
	 <addDOCr222>
	  <addDOCc444>addDOCc444</addDOCc444>addDOCr222</addDOCr222>

	*/	
	public String  processReplace(String NID, boolean enableReplaceDocument){
		String returnNID="";
		try {
			int testCase=1;
			String objectType=this.objectType;
			String rootChildXpath=".";
			String childXpath="";
			String whereXpath="";
			String childXml="";
			Hashtable<String,String> replaceList = new Hashtable<String, String>();
			
			Document doc = MRConfig.getInstance(objectType, log).getXmlRepresentation(log).getDoc();
			childXml = MRXMLUtils.documentToString(doc );

			if(enableReplaceDocument){
				//using docNID replace Document
				testCase=6;
				replaceList.put("//TagName", "updateTagName"+ testCase);
				whereXpath="//replaceLISTc33[.='replaceLISTc33']";
				doc = replace(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
				
				//using list replace all document
				testCase=1;
				replaceList.put("//TagName", "updateTagName"+ testCase);
				doc = replace(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
				
				return "";
			}	
			
			
			//to childXpath + with docs for where clause in list
			testCase=10;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addLISTrwc11/addLISTc33";
			whereXpath="//addLISTc33[.='addLISTc33']";
			childXml="<replaceLISTc33>replaceLISTc33</replaceLISTc33>";
			doc = replace(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);
			
			//to childXpath + all docs in list 
			testCase=9;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addDOCrwc11/addDOCc33";
			childXml="<replaceDOCc33>replaceDOCc33</replaceDOCc33>";
			doc = replace(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);
				
			//to root + with docs for where clause in list
			testCase=8;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addLISTr22";
			whereXpath="//replaceDOCc33[.='replaceDOCc33']";
			childXml="<replaceLISTr22><newaddLISTc44>newaddLISTc44</newaddLISTc44>replaceLISTr22</replaceLISTr22>";
			doc = replace(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);
			
			//to root + all docs in list 
			testCase=7;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addDOCr22";
			childXml="<replaceDOCr22><newaddDOCc44>newaddDOCc44</newaddDOCc44>replaceDOCr22</replaceDOCr22>";
			doc = replace(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);

			/////////////////////// using list - add child   ///////////////////////////////////////
			//to childXpath + with docs for where clause in list
			testCase=5;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addLISTrwc1/addLISTc3";
			whereXpath="//addLISTc3[.='addLISTc3']";
			childXml="<replaceLISTc3>replaceDoc3</replaceLISTc3>";
			doc = replace(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);
			
			//to childXpath + all docs in list 
			testCase=4;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addDOCrwc1/addDOCc3";
			childXml="<replaceDOCc3>replaceDOCc3</replaceDOCc3>";
			doc = replace(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);

			//to root + with docs for where clause in list
			testCase=3;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addLISTr2";
			whereXpath="//addLISTr2[.='addLISTr2']";
			childXml="<replaceLISTr2><newaddLISTc4>newaddLISTc4</newaddLISTc4>replaceLISTr2</replaceLISTr2>";
			doc = replace(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);

			//to root + all docs in list 
			testCase=2;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addDOCr2";
			childXml="<replaceDOCr2><newaddDOCc4>newaddDOCc4</newaddDOCc4>replace1</replaceDOCr2>";
			doc = replace(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);
			

			/////////////////////// using list - add child   ///////////////////////////////////////
		
		} catch (Exception e) {
			e.printStackTrace();
		}
		return returnNID;
	}	
	public Document replace ( int testCase, String objectType, String NID, String rootChildXpath, String childXpath, String  whereXpath, String childXml , Hashtable<String,String> replaceList ) {
		Document doc = null;
		try{
			String ID=objectType+"_update_"+testCase;
			String xmlFilePath=objectType+"_BASE_ALL_UPDATE.xml";;
			doc = AssetAdminServiceUtils.getAssetAdminRequest(log, xmlFilePath);
			
			//replace operationData
			Element operationDataReq = MRXMLUtils.stringToElement(childXml);
			Element operationData = MRXMLUtils.getElementXpath(doc, "//OperationData");
			Element parent = MRXMLUtils.getParentElement(operationData);
			operationData.detach();
			parent = MRXMLUtils.addElement(parent, "OperationData");
			MRXMLUtils.addElement(parent, operationDataReq);
			
			
			//replace if any values from hashtable
			for (String xpath : replaceList.keySet()) {
				String value = replaceList.get(xpath);
				MRXMLUtils.setValueXpath(doc, xpath, value);
			}	
			
			switch(testCase){
				//////////////////// list//////////////////////////// 
				case 1: //documents
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					break;
				//////////////////// list//////////////////////////// 
				case 2: //parent-as-root  
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/In/ChildXPath", rootChildXpath);
					break;
				case 3:  //parent-as-root + where
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/In/ChildXPath", rootChildXpath);
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/Where/WhereXPath", whereXpath);
					break;	
				case 4: //parent-as-xpath   
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/In/ChildXPath", childXpath);
					break;
				case 5:  //parent-as-xpath + where
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/In/ChildXPath", childXpath);
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/Where/WhereXPath", whereXpath);
					break;	
					
				//////////////////// docNID//////////////////////////// 
				case 6: //documents
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/NID", NID);
					break;
				case 7: //parent-as-root 
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/NID", NID);
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/In/ChildXPath", rootChildXpath);
					break;
				case 8:  //parent-as-root + where
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/NID", NID);
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/In/ChildXPath", rootChildXpath);
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/Where/WhereXPath", whereXpath);
					break;	
				case 9: //parent-as-xpath
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/NID", NID);
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/In/ChildXPath", childXpath);
					break;
				case 10:  //parent-as-xpath + where
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/NID", NID);
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/In/ChildXPath", childXpath);
					MRXMLUtils.setValueXpath(doc, "//ReplaceRequest/PathSpec/Where/WhereXPath", whereXpath);
					break;	
				//////////////////// add to parent xpath//////////////////////////// 
	 			default:
					break;
			}
			doc = sendRequest(ID, doc);
			
		}catch(Exception ex){
			ex.printStackTrace();
		}
		return doc ;
	}
	////////////////////////REPLACE  ////////////////////////////////////////////////////////////////////////
	////////////////////////REPLACE  ////////////////////////////////////////////////////////////////////////
	////////////////////////REPLACE  ////////////////////////////////////////////////////////////////////////

	
	
	////////////////////////DELETE  ////////////////////////////////////////////////////////////////////////
	////////////////////////DELETE  ////////////////////////////////////////////////////////////////////////
	////////////////////////DELETE  ////////////////////////////////////////////////////////////////////////
	/*
	 1- using list  
	 	-delete documents
	 	-delete child
		 	-to root + all docs in list 
		 		-to root + with docs for where clause in list
		 	-to childXpath + all docs in list 
		 		-to childXpath + with docs for where clause in list
	 	
	 2- using docNID
	 	-delete document
	 	-delete child
		 	-to root + all docs in list 
		 		-to root + with docs for where clause in list
		 	-to childXpath + all docs in list 
		 		-to childXpath + with docs for where clause in list
	 	
	 */		
	/*
	<addLISTrwc1><addLISTc3>addLISTc3</addLISTc3>addLISTrwc1</addLISTrwc1>
	<addLISTr2><addLISTc4>addLISTc4</addLISTc4>addLISTr2</addLISTr2>
	<addDOCrwc1><addDOCc3>addDOCc3</addDOCc3>addDOCrwc1</addDOCrwc1>
	<addDOCr2><addDOCc4>addDOCc4</addDOCc4>addDOCr2</addDOCr2>
	<addLISTrwc11><addLISTc33>addLISTc33</addLISTc33>addLISTrwc11</addLISTrwc11>
	<addLISTr22><addLISTc44>addLISTc44</addLISTc44>addLISTr22</addLISTr22>
	<addDOCrwc11><addDOCc33>addDOCc33</addDOCc33>addDOCrwc11</addDOCrwc11>
	<addDOCr22><addDOCc44>addDOCc44</addDOCc44>addDOCr22</addDOCr22>
	<addLISTrwc111>
	 <addLISTc333>addLISTc333</addLISTc333>addLISTrwc111</addLISTrwc111>
	<addLISTr222>
	 <addLISTc444>addLISTc444</addLISTc444>addLISTr222</addLISTr222>
	<addDOCrwc111>
	 <addDOCc333>addDOCc333</addDOCc333>addDOCrwc111</addDOCrwc111>
	<addDOCr222>
	 <addDOCc444>addDOCc444</addDOCc444>addDOCr222</addDOCr222>
	
	*/
	public String  processDelete(String NID, boolean enableReplaceDocument){
		String returnNID="";
		try {
			int testCase=1;
			String objectType=this.objectType;
			String rootChildXpath=".";
			String childXpath="";
			String whereXpath="";
			String childXml="";
			Hashtable<String,String> replaceList = new Hashtable<String, String>();
 
			Document doc = MRConfig.getInstance(objectType, log).getXmlRepresentation(log).getDoc();
			childXml = MRXMLUtils.documentToString(doc );

			if(enableReplaceDocument){
				//using docNID replace Document
				testCase=6;
				replaceList.put("//TagName", "updateTagName"+ testCase);
				whereXpath="//TagName[.='updateTagName1']";
				doc = delete(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
				
				//using list replace all document
				testCase=1;
				replaceList.put("//TagName", "updateTagName"+ testCase);
				doc = delete(testCase, objectType, NID, rootChildXpath, childXpath, whereXpath, childXml, replaceList);
				
				return "";
			}	
			
			
			//to childXpath + with docs for where clause in list
			testCase=10;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addLISTrwc11/replaceLISTc33";
			whereXpath="//replaceLISTc33[.='replaceLISTc33']";
			//childXml="<replaceLISTc33>replaceLISTc33</replaceLISTc33>";
			doc = delete(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);
			
			//to childXpath + all docs in list 
			testCase=9;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addDOCrwc11/replaceDOCc33";
			//childXml="<replaceDOCc33>replaceDOCc33</replaceDOCc33>";
			doc = delete(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);
				
			//to root + with docs for where clause in list
			testCase=8;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="replaceLISTr22";
			whereXpath="//newaddLISTc44[.='newaddLISTc44']";
			//childXml="<replaceLISTr22><newaddLISTc44>newaddLISTc44</newaddLISTc44>replaceLISTr22</replaceLISTr22>";
			doc = delete(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);
			
			//to root + all docs in list 
			testCase=7;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="replaceDOCr22";
			//childXml="<replaceDOCr22><newaddDOCc44>newaddDOCc44</newaddDOCc44>replaceDOCr22</replaceDOCr22>";
			doc = delete(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);

			/////////////////////// using list - add child   ///////////////////////////////////////
			//to childXpath + with docs for where clause in list
			testCase=5;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addLISTrwc1/replaceLISTc3";
			whereXpath="//replaceLISTc3[.='replaceLISTc3']";
			//childXml="<replaceLISTc3>replaceDoc3</replaceLISTc3>";
			doc = delete(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);
			
			//to childXpath + all docs in list 
			testCase=4;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="addDOCrwc1/replaceDOCc3";
			//childXml="<replaceDOCc3>replaceDOCc3</replaceDOCc3>";
			doc = delete(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);

			//to root + with docs for where clause in list
			testCase=3;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="replaceLISTr2";
			whereXpath="//newaddLISTc4[.='newaddLISTc4']";
			//childXml="<replaceLISTr2><newaddLISTc4>newaddLISTc4</newaddLISTc4>replaceLISTr2</replaceLISTr2>";
			doc = delete(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);

			//to root + all docs in list 
			testCase=2;
			replaceList.put("//TagName", "updateTagName"+ testCase);
			childXpath="replaceDOCr2";
			//childXml="<replaceDOCr2><newaddDOCc4>newaddDOCc4</newaddDOCc4>replace1</replaceDOCr2>";
			doc = delete(testCase, objectType, NID, childXpath, childXpath, whereXpath, childXml, replaceList);
			

			/////////////////////// using list - add child   ///////////////////////////////////////
		} catch (Exception e) {
			e.printStackTrace();
		}
		return returnNID;
	}	
 	
	public Document delete ( int testCase, String objectType, String NID, String rootChildXpath, String childXpath, String  whereXpath, String childXml , Hashtable<String,String> replaceList ) {
		Document doc = null;
		try{
			String ID=objectType+"_delete_"+testCase;
			String xmlFilePath=objectType+"_BASE_ALL_DELETE.xml";;
			doc = AssetAdminServiceUtils.getAssetAdminRequest(log, xmlFilePath);
			
			//replace if any values from hashtable
			for (String xpath : replaceList.keySet()) {
				String value = replaceList.get(xpath);
				MRXMLUtils.setValueXpath(doc, xpath, value);
			}	
			
			switch(testCase){
				//////////////////// list//////////////////////////// 
				case 1: //documents
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					break;
				//////////////////// list//////////////////////////// 
				case 2: //parent-as-root  
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/In/ChildXPath", rootChildXpath);
					break;
				case 3:  //parent-as-root + where
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/In/ChildXPath", rootChildXpath);
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/Where/WhereXPath", whereXpath);
					break;	
				case 4: //parent-as-xpath   
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/In/ChildXPath", childXpath);
					break;
				case 5:  //parent-as-xpath + where
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/NID", mServiceTestUtil.getParentListNID(objectType, NID));
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/In/ChildXPath", childXpath);
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/Where/WhereXPath", whereXpath);
					break;	
					
				//////////////////// docNID//////////////////////////// 
				case 6: //documents
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/NID", NID);
					break;
				case 7: //parent-as-root 
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/NID", NID);
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/In/ChildXPath", rootChildXpath);
					break;
				case 8:  //parent-as-root + where
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/NID", NID);
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/In/ChildXPath", rootChildXpath);
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/Where/WhereXPath", whereXpath);
					break;	
				case 9: //parent-as-xpath
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/NID", NID);
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/In/ChildXPath", childXpath);
					break;
				case 10:  //parent-as-xpath + where
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/NID", NID);
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/In/ChildXPath", childXpath);
					MRXMLUtils.setValueXpath(doc, "//DeleteRequest/PathSpec/Where/WhereXPath", whereXpath);
					break;	
				//////////////////// add to parent xpath//////////////////////////// 
	 			default:
					break;
			}
			doc = sendRequest(ID, doc);
			
		}catch(Exception ex){
			ex.printStackTrace();
		}
		return doc ;
	}	
	////////////////////////DELETE  ////////////////////////////////////////////////////////////////////////
	////////////////////////DELETE  ////////////////////////////////////////////////////////////////////////
	////////////////////////DELETE  ////////////////////////////////////////////////////////////////////////
	 
	
	
	
	////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////
	////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////
	////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////
 
	public boolean login() {
		MRClientAgent.setLegacyResponse(true);
		if(!mServiceTestUtil.doLogin_File(loginXML)){
			//System.out.println("FAILED Login: " + mServiceTestUtil.getLoginResponse()  );
 			return false;
		}
		if(!mServiceTestUtil.doSetServiceDomain_File( setDomainXML )){
			//System.out.println("FAILED SetServiceDomain: " + mServiceTestUtil.getLoginResponse()  );
			return false;
		}
		return true;
	}
	public boolean logout() {
		return mServiceTestUtil.doLogout();
	}
	public Document sendRequest(String ID, Document doc) {
		long t0 = System.currentTimeMillis();
		//long tLoop = 0;
		boolean waitforResult=true;
		Document docRet =null;
		String respXml = "";
		//System.out.println(  ID + "////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////");
		//System.out.println(  ID + "////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////");
		//System.out.println(  ID + "////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////");
		try {
			////System.out.println("request " + MRXMLUtils.documentToString(doc));
			Element clientdata = MRXMLUtils.getElementXpath(doc, "//header/clientdata");
			if(clientdata !=null){
				clientdata.setText( getNextClientData(ID));
			}
			try {
				//System.out.println(  " --> \n<!--" + ID + "////////////////////////Request  ////////////////////////////////////////////////////////////////////////--> \n<!--");
				System.out.println( " -->\n" + MRXMLUtils.elementToString(doc.getRootElement()) +"\n<!--" );
				ServiceTestUtil.RespXML resp = mServiceTestUtil.doSend(MRXMLUtils.documentToString(doc), " GetRequest ", waitforResult);
				if (resp.got_resp == 1) {
					Thread.sleep(50);
					respXml = resp.xml;
					docRet= MRXMLUtils.stringToDocument(respXml);
				}
			} catch (Exception ex) {
				ex.printStackTrace();
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		} finally {
			////System.out.println(  ID + " END - time: diff: " + (System.currentTimeMillis() - t0) + " - " + System.currentTimeMillis());
		}
		//////System.out.println("respXml " + respXml);
		//System.out.println(  " --> \n<!--" + ID + "////////////////////////Response  ////////////////////////////////////////////////////////////////////////--> \n<!--");
		try {
			System.out.println( " -->\n" + MRXMLUtils.elementToString(docRet.getRootElement()) +"\n<!--" );
		} catch (Exception e) {
			e.printStackTrace();
		}
		//System.out.println(  ID + "////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////");
		//System.out.println(  ID + "////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////");
		//System.out.println(  ID + "////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////");
		
		return docRet;
	}	
 
 
	////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////
	////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////
	////////////////////////sendRequest  ////////////////////////////////////////////////////////////////////////

	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
	public static final Random wheel = new Random();
	private String getNextClientData(String seed){
		return  seed +"_"+ System.currentTimeMillis() +"_"+ (Integer.toHexString(wheel.nextInt()));
	}
	public void initProperties(String loadType, String propFile) {
		try {
			/*
				############login ################
				loginXML=/com/ipvs/test/xml/StressTestDB_login.xml
				setDomainXML=/com/ipvs/test/xml/StressTestDB_setServiceDomain.xml
				 
 			 */
			log = MRLog.getInstance(MRLog.CONSOLE);
			props= new Properties();
			try {
				props = Utils.loadProperties(propFile);
			} catch (Exception ex) {
				try {
					props = Utils.loadPropertiesResource(props, propFile, this.getClass());
				} catch (Exception ex2) {
					ex.printStackTrace();
					ex2.printStackTrace();
				}
			}

			loginXML = props.getProperty("loginXML", "").trim();
			setDomainXML = props.getProperty("setDomainXML", "").trim();
			objectType = props.getProperty("objecttype", "").trim();
			
		} catch (Exception ex) {

		}
	}
	 
	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
	////////////////////////properties file ////////////////////////////////////////////////////////////////////////
}