/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the  "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * $Id: SimpleTransform.java 470245 2006-11-02 06:34:33Z minchau $
 */
package com.ipvsserv.nxd.xslt;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.Hashtable;
import java.util.Vector;

import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import com.ipvsserv.common.util.CompUtils;

/**
 *  Use the TraX interface to perform a transformation in the simplest manner possible
 *  (3 statements).
 */
public class XsltTransformUtils {
//    public static void main2(String[] args) throws TransformerException, TransformerConfigurationException, FileNotFoundException, IOException {
//        // Use the static TransformerFactory.newInstance() method to instantiate 
//        // a TransformerFactory. The javax.xml.transform.TransformerFactory 
//        // system property setting determines the actual class to instantiate --
//        // org.apache.xalan.transformer.TransformerImpl.
//        TransformerFactory tFactory = TransformerFactory.newInstance();
//
//        // Use the TransformerFactory to instantiate a Transformer that will work with  
//        // the stylesheet you specify. This method call also processes the stylesheet
//        // into a compiled Templates object.
//        Transformer transformer = tFactory.newTransformer(new StreamSource("src/dbmigration/com/ipvsserv/nxd/migration/xslt/birds.xsl"));
//
//        // Use the Transformer to apply the associated Templates object to an XML document
//        // (foo.xml) and write the output to a file (foo.out).
//        transformer.transform(new StreamSource("src/dbmigration/com/ipvsserv/nxd/migration/xslt/birds.xml"), new StreamResult(new FileOutputStream("src/dbmigration/com/ipvsserv/nxd/migration/xslt/birds.out")));
//
//        System.out.println("************* The result is in birds.out *************");
//    }

    public static void transform_xsltproc(String xsl, File srcFile, File destFile) throws Exception {

        Vector<String> vec = new Vector<String>();
        vec.add("xsltproc");
        vec.add("-o");
        vec.add(destFile.getCanonicalPath());
        vec.add(xsl);
        vec.add(srcFile.getAbsolutePath());

        String[] cmdArgs = new String[vec.size()];
        vec.copyInto(cmdArgs);
        //xsltproc -o $i.tmp --stringparam serviceVersion $serviceVersion ../utils/xsl/update_object_version.xsl $i
        StringBuffer buf = new StringBuffer();
        int exitVal = 0;
        exitVal = CompUtils.executeProcess(cmdArgs, buf);

        if (exitVal != 0) {
            //script should not return non zero value.
            throw new Exception("ERROR "+CompUtils.joinArray(cmdArgs, " ")+" "+buf.toString());
        }

    }
    public static void transform_Xalan(String xsl, File srcFile, File destFile) throws TransformerException, TransformerConfigurationException, FileNotFoundException, IOException {
        transform_Xalan(xsl, srcFile, destFile, null);
    }
    public static void transform_Xalan(String xsl, File srcFile, File destFile, Hashtable<String, String> parameters) throws TransformerException, TransformerConfigurationException, FileNotFoundException, IOException {

        // Use the static TransformerFactory.newInstance() method to instantiate 
        // a TransformerFactory. The javax.xml.transform.TransformerFactory 
        // system property setting determines the actual class to instantiate --
        // org.apache.xalan.transformer.TransformerImpl.
        TransformerFactory tFactory = TransformerFactory.newInstance();

        // Use the TransformerFactory to instantiate a Transformer that will work with  
        // the stylesheet you specify. This method call also processes the stylesheet
        // into a compiled Templates object.
        Transformer transformer = tFactory.newTransformer(new StreamSource(xsl));
        
        if(parameters !=null) {
            for (String parameter : parameters.keySet()) {
                transformer.setParameter(parameter, parameters.get(parameter));
            }
        }
        // Use the Transformer to apply the associated Templates object to an XML document
        // (foo.xml) and write the output to a file (foo.out).
        FileOutputStream os = new FileOutputStream(destFile);
        try{
        transformer.transform(new StreamSource(srcFile), new StreamResult(os));
        }finally{
            try{
                os.close();
            }catch(Exception ex){}
            try{
                os.close();
            }catch(Exception ex){}
            os=null;
        }
        //System.out.println("************* The result is in " + outFile.getAbsolutePath() + " *************");

    }

    public static void transform_Xalan(String xsl, StringReader srcFile, StringWriter destFile, Hashtable<String, String> parameters) throws TransformerException, TransformerConfigurationException, FileNotFoundException, IOException {

        // Use the static TransformerFactory.newInstance() method to instantiate 
        // a TransformerFactory. The javax.xml.transform.TransformerFactory 
        // system property setting determines the actual class to instantiate --
        // org.apache.xalan.transformer.TransformerImpl.
        TransformerFactory tFactory = TransformerFactory.newInstance();

        // Use the TransformerFactory to instantiate a Transformer that will work with  
        // the stylesheet you specify. This method call also processes the stylesheet
        // into a compiled Templates object.
        Transformer transformer = tFactory.newTransformer(new StreamSource(xsl));
        // Use the Transformer to apply the associated Templates object to an XML document
        // (foo.xml) and write the output to a file (foo.out).
        for (String parameter : parameters.keySet()) {
            transformer.setParameter(parameter, parameters.get(parameter));
        }
        transformer.transform(new StreamSource(srcFile), new StreamResult(destFile));
        
        //System.out.println("************* The request is in " + srcFile.toString() + " *************");
        //System.out.println("************* The result is in " + destFile.toString() + " *************");

    }

}
