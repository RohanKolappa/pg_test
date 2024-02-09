package com.barco.utils.mr;

import org.dom4j.Document;

public class MRMessage {
	
	protected Document document;
	
	public MRMessage(Document document) {
		this.document = document;
	}
	
	public Document getDocument() {
		return this.document;
	}

}
