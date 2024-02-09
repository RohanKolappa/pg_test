package com.barco.nimbus.utils.example.dom4j;

import javax.ws.rs.QueryParam;

import org.dom4j.Document;

public interface IDOM4J {

	Document stringToDocument(@QueryParam("xml") String s) throws Exception;

}
