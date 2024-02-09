package com.ipvs.nimbus.db;


public class SednaToExistTranslation {
/*	
	
    public static String getCollectionConextFromNIDVar(String domainName, String NIDVariable) {
        if(IpvsServGlobals.isUseListCol()){
            if(domainName==null || domainName.equals("")){
                MRLog.debug("getCollectionConextFromNIDVar domain Name ????'" + domainName+"'");
                domainName=IpvsServGlobals.getDomainDefault();
            }
            //String parentCol = "concat('/db/ipvs.default.',  substring-before( substring-after( "+NIDVariable+", 'default.') , '.'  )  )";
            String docuri = "concat('/db/"+IpvsServGlobals.getListColPrefix()+""+domainName+".',  substring-before( substring-after( "+NIDVariable+", '"+domainName+".') , '.'  ) , '/', substring-after( substring-after( "+NIDVariable+", '"+domainName+".'),'.'))";
             //return " collection(" + parentCol+ ")/*";
            return " doc(" + docuri+ ")/*";
        }
        return " collection('" + IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_PREFIX) + domainName + "')/*";
    }   
    public static String getCollectionConextFromParentNIDVar(String domainName, String NIDVariable) {
        if(IpvsServGlobals.isUseListCol()){
            return " collection(concat('"+IpvsServGlobals.getListColPrefix()+"'," + NIDVariable+ "))/*";
        }
        return " collection('" + IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_PREFIX) + domainName + "')/*";
    }     
    public static String getCollectionConext(String domainName, String NID) {
        if(IpvsServGlobals.isUseListCol()){
             //todo:pass NIDSchemeMDC as argument
             NIDSchemeMDC node = new NIDSchemeMDC();
             node.parse(NID);
             if(node.getType()==NIDSchemeMDC.COLLECTION) {
                 return " collection('/db/" + node.getCollectionName()+ "')/*";
             } else if(node.getType()==NIDSchemeMDC.DOCUMENT) {
                 return " doc('/db/" + node.getCollectionName()+ "/" + node.getDocUri() +"')/*";
             }  
             return null;
        }
        return " collection('" + IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_PREFIX) + domainName + "')/*";
    }

    public static String sednaToExistQueryContext(String domainName, String context) throws Exception {
        String docIndex = "IPVSi_nids_";
        String colIndex = "IPVSis_colnids_";
        //String colIndex= IpvsServGlobals.getDomainDBSchema().getIndexNamePrefix("nodeColIndex");
        //String docIndex= IpvsServGlobals.getDomainDBSchema().getIndexNamePrefix("nodeIndex");
        int ipos = context.indexOf("(");
        if (ipos == -1) {
            throw new Exception("Invalid context '" + context + "'");
        }
        int ipos2 = context.lastIndexOf(")");
        if (ipos2 == -1) {
            throw new Exception("Invalid context '" + context + "'");
        }

        String params = context.substring(ipos + 1, ipos2);
        //System.out.println("sednaToExistQueryContext params " + params);
        String[] arr = params.split(",");
        String indexName = arr[0].replace('\'', ' ').trim();
        String nid = arr[1].replace('\'', ' ').trim();
        if (indexName.startsWith(colIndex)) {
            //domainName = indexName.substring(colIndex.length());
            if (nid.startsWith("$")) {
                return SednaToExistTranslation.getCollectionConextFromParentNIDVar(domainName, nid) + "[@parentNID=" + nid.trim() + "]";
            } else {
                return SednaToExistTranslation.getCollectionConext(domainName, nid) + "[@parentNID='" + nid.trim() + "']";
            }
        } else if (indexName.startsWith(docIndex)) {
            //domainName = indexName.substring(docIndex.length());
            if (nid.startsWith("$")) {
                return SednaToExistTranslation.getCollectionConextFromNIDVar(domainName, nid) + "[@NID=" + nid.trim() + "]";
            } else {
                return SednaToExistTranslation.getCollectionConext(domainName, nid) + "[@NID='" + nid.trim() + "']";
            }
        } else {
            throw new Exception("Invalid context '" + context + "'");
        }
    }

	
	 * TBD MIYE
	 * Legacy sedna code uses index-scan, this code hopefully converts the xquery 
	 
    public static String sednaToExistQuery_indexscan(String domainName, String query) throws Exception {
        StringBuffer buf = new StringBuffer(100);
        String indexSecan = "index-scan";
        try {
            int len = query.length();
            for (int i = 0; i < len; i++) {
                char c = query.charAt(i);
                if (c == 'i') {
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
                        if (indexSecan.equals(localBuf.toString())) {
                            int ipos = query.indexOf(")", i);
                            if (ipos == -1) {
                                throw new Exception("Invalid index-scan");
                            }
                            ipos = ipos + ")".length();
                            localBuf.append(query.substring(i + 1, ipos));
                            i = ipos;

                            /////////////// get next chars which will look like /..
                            int count = 0;
                            StringBuffer localBuf2 = new StringBuffer();

                            while (true) {
                                if (ipos + 1 > len) {
                                    break;
                                }
                                c = query.charAt(ipos);
                                if (Character.isWhitespace(c)) {
                                } else {
                                    localBuf2.append(c);
                                    count++;
                                }
                                ipos++;
                                if (count == 3) {
                                    break;
                                }
                            }
                            if (localBuf2.toString().equals("/..")) {
                                localBuf.append(localBuf2.toString());
                                i = ipos - 1;
                            }
                            buf.append(sednaToExistQueryContext(domainName, localBuf.toString()));
                            break;
                        } else if (indexSecan.startsWith(localBuf.toString())) {
                            //keep reading
                        } else {
                            buf.append(localBuf.toString());
                            break;
                        }
                    }
                } else {
                    buf.append(c);
                }
            }
        } finally {
            //System.out.println("-----------------after fix='" + buf.toString() + "'-----------------");
        }
        return buf.toString();
    }    */

}
