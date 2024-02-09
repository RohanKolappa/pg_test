package com.ipvs.common.db;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRDB;

public class DBUtilsBase {
	
	public static final String PREFIX_OBJ="#OBJ_";
	public static final String SUFFIX_OBJ="_OBJ#";
	public static final int PREFIX_OBJ_LEN=PREFIX_OBJ.length();
	public static final int SUFFIX_OBJ_LEN=SUFFIX_OBJ.length();
	
	public static IpvsDBI theIpvsDB = null;
	public static boolean forceXMPP = false;
	
	public static String updateNIDXPathList(String query, Map<String, String> pemXPathList, MRLog log, MRDB db) throws Exception {

		Map<String, String> nidList = new Hashtable<String, String>();
		for (Iterator<String> iterator = pemXPathList.keySet().iterator(); iterator.hasNext();) {
			String objectType = iterator.next();
			if (objectType.indexOf('.') != -1) {
				NIDSchemeMDC schema = new NIDSchemeMDC();
				schema.parse(objectType);
				if (schema.getType() == NIDSchemeMDC.COLLECTION) {
					nidList.put(objectType, schema.getCollectionNodeID());
				} else if (schema.getType() == NIDSchemeMDC.DOCUMENT) {
					nidList.put(objectType, schema.getDocNodeID());
				} else {
					throw new Exception(" invalid NID or Objectype " + objectType);
				}
			} else {
				MRConfig config = MRConfig.getInstance(objectType, log);
				String parentKey = config.getParentNIDKey(log);
				String nid = db.getServiceDomainRootNID(parentKey);
				if (nid == null) {
					throw new Exception(" invalid NID or Objectype  '" + objectType + "' parentKey='" + parentKey + "'");
				}
				nidList.put(objectType, nid);
			}
		}

		int startPos = 0;
		StringBuffer buf = new StringBuffer(100);
		int pos1 = query.indexOf(DBUtilsBase.PREFIX_OBJ, startPos);

		int pos2 = 0;
		if (pos1 == -1) {
			return query;
		}
		while (pos1 != -1) {
			if (pos1 > startPos) {
				buf.append("\n" + query.substring(startPos, pos1));
			}
			pos2 = query.indexOf(DBUtilsBase.SUFFIX_OBJ, pos1);
			if (pos2 == -1) {
				throw new Exception("Invalid QueryRequest SUFFIX_OBJ not found. " + query);
			}
			String objectType = query.substring(pos1 + DBUtilsBase.PREFIX_OBJ_LEN, pos2);
			if (!nidList.containsKey(objectType)) {
				throw new Exception("Invalid QueryRequest nid not found for " + objectType + " " + query);
			}

			String xpath = null;
			List<String> arrXpath = new ArrayList<String>();
			if (pemXPathList.containsKey(objectType)) {
				xpath = pemXPathList.get(objectType);
				if (!xpath.trim().equals("")) {
					arrXpath.add(xpath);
				}
			}
			String NID = nidList.get(objectType);
			buf.append(getNodeXpathContext("readQuery-" + objectType, "default", 1, "", NID, arrXpath, ""));

			startPos = pos2 + DBUtilsBase.SUFFIX_OBJ_LEN;
			pos1 = query.indexOf(DBUtilsBase.PREFIX_OBJ, startPos);
		}
		if (query.length() > startPos) {
			buf.append(query.substring(startPos));
		}

		return buf.toString();
	}

	public static String getNodeXpathContext(String ID, String domainName, int nodeType, String indexName, String NID, List<String> arrXpath, String childContext) throws Exception {
		NIDSchemeMDC node = new NIDSchemeMDC(); 
		node.parse(NID);
		if (node.getType() == NIDSchemeMDC.COLLECTION) {
			arrXpath.add("@parentNID='" + NID.trim() + "'");
		} else {//if (nodeType == NIDSchemeMDC.DOCUMENT)
			arrXpath.add("@NID='" + NID.trim() + "'");
		}
		StringBuffer buf = new StringBuffer();
		buf.append(getCollectionConext(domainName, NID));
		buf.append(getWhereXpath(arrXpath));
		buf.append(prefixContext(childContext));
		return buf.toString();
	}
	
	private static String prefixContext(String childContext) {
		if (childContext == null)
			return "";
		childContext = childContext.trim();
		if (childContext.length() > 0) {
			if (childContext.equals(".")) {
				return "";
			} else if (!childContext.trim().startsWith("/")) {
				return "/" + childContext.trim();
			}
		}
		return childContext;
	}

	public static String getCollectionConext(String domainName, String NID) throws Exception {
		NIDSchemeMDC node = new NIDSchemeMDC(); 
		node.parse(NID);
		if(node.getType()==NIDSchemeMDC.COLLECTION) {
			return " collection('/db/ipvs." + NID + "')/*";
		} else if(node.getType()==NIDSchemeMDC.DOCUMENT) {
			return " doc('/db/ipvs." + NID + "/" + node.getDocUri() +"')/*";
		}  
		return null;
	}

	// This is same logic as the next method, not sure why the input is a List instead of an Array
	public static String convertArrayToXpath(String[] arrXpath) {
		if (arrXpath == null || (arrXpath.length == 0)) {
			return "";
		}
		StringBuffer buf = new StringBuffer();
		boolean bAddAND = false;
		boolean bAdded = false;
		for(String xpath : arrXpath) {
			if (xpath != null && xpath.trim().length() > 0) {
				if (bAddAND) {
					buf.append(" and ");
				} else {
					bAdded = true;
					buf.append("[");
					bAddAND = true;
				}
				buf.append("(" + replaceContext(xpath.trim()) + ")");
			}
		}
		if (bAdded) {
			buf.append("]");
		}
		return buf.toString();
		
	}
    
	public static String convertArrayToSortByXpath(String[] arrXpath) {
		if (arrXpath == null || (arrXpath.length == 0)) {
			return "";
		}
		StringBuffer buf = new StringBuffer();
		boolean bAddAND = false;
		for(String xpath : arrXpath) {
			if (xpath != null && xpath.trim().length() > 0) {
				if (bAddAND) {
					buf.append(",");
				} else {
					bAddAND = true;
				}
				buf.append(xpath.trim());
			}
		}
		return buf.toString();
		
	}
	private static String getWhereXpath(List<String> arrXpath) {
		if (arrXpath == null || arrXpath.isEmpty()) {
			return "";
		}
		StringBuffer buf = new StringBuffer();
		boolean bAddAND = false;
		boolean bAdded = false;
		for (Iterator<?> i = arrXpath.iterator(); i.hasNext();) {
			String xpath = (String) i.next();
			if (xpath != null && xpath.trim().length() > 0) {
				if (bAddAND) {
					buf.append(" and ");
				} else {
					bAdded = true;
					buf.append("[");
					bAddAND = true;
				}
				buf.append("(" + replaceContext(xpath.trim()) + ")");
				/*if(xpath.startsWith("//*[@parentNID") || xpath.startsWith("//*[@NID")){
                   buf.append("(" + xpath.trim() + ")");
               }else{
                   buf.append("(" + xpath.trim().replace("//", ".//") + ")");
               }*/
			}
		}
		if (bAdded) {
			buf.append("]");
		}
		return buf.toString();
	}

	private static String replaceContext(String query) {
		Matcher m = Pattern.compile("([^a-zA-Z0-9.]//|^//)").matcher(query);
		StringBuffer sb = new StringBuffer();
		while(m.find()) {
			String ss = m.group(0);
			ss = ss.replaceAll("//", ".//");
			m.appendReplacement(sb, ss);
		}
		m.appendTail(sb);
		//System.out.println("'REGED='" + sb.toString() + "' " + "=========ORIG='"+query );
		return sb.toString();

	}
	
	
	  public static String convertXPaths(String query) throws Exception {
	        //System.out.println("-----------------before fix='" + query + "'-----------------");
	        StringBuffer buf = new StringBuffer(100);
	        try {
	            int len = query.length();
	            for (int i = 0; i < len; i++) {
	                char c = query.charAt(i);
	                if (c == '(') {
	                    if (i + 1 < len) {
	                        char c2 = query.charAt(i + 1);
	                        if (c2 == ':') {
	                            int ipos = query.indexOf(":)", i);
	                            if (ipos == -1) {
	                                throw new Exception("Invalid comment");
	                            }
	                            i = ipos + ":)".length() - 1;
	                            continue;
	                        } else {
	                            buf.append(c);
	                            continue;
	                        }
	                    } else {
	                        buf.append(c);
	                        continue;
	                    }

	                } else if (c == '[') {
	                    int closeBracketCounter=1;
	                    StringBuffer localBuf = new StringBuffer();
	                    localBuf.append(c);
	                    while (true) {
	                        if (i + 1 > len) {
	                            buf.append(localBuf.toString());
	                            break;
	                        }
	                        i++;
	                        c = query.charAt(i);
	                        localBuf.append(c);
	                        if (c == '[') {
	                            closeBracketCounter++; 
	                        }
	                        if (c == ']') {
	                            closeBracketCounter--; 
	                        }
	                        if(closeBracketCounter==0){
	                             break;
	                        }
	                    }
	                    
	                    //buf.append(localBuf.toString().replace("//", ".//"));
	                    buf.append(replaceContext(localBuf.toString()) );
                    
	                } else {
	                    buf.append(c);
	                }
	            }
	        } finally {
	            //System.out.println("-----------------after fix='" + buf.toString() + "'-----------------");
	        }
	        return buf.toString();
	    }



}
