package com.barco.utils.mr;

import org.dom4j.Element;

public class AuditEvent {
	
	//Mandatory fields
	String type = "";
	String name = "";
	String userJID = "";
	String level = "";
	
	//Optional fields
	String descriptionText;
	Element operatorEl;
	Element targetEl;
	Element descriptionXML;
	String status = "";
	String target = "";


	public AuditEvent(Builder builder) {		
		this.type=builder.type;
		this.name=builder.name;
		this.status = builder.status;
		this.userJID = builder.userJID;
		this.target = builder.target;
		this.level = builder.level;
		this.descriptionText = builder.descriptionText;
		this.operatorEl = builder.operatorEl;
		this.targetEl = builder.targetEl;
		this.descriptionXML = builder.descriptionXML;
	}

	//used builder design pattern to get better readability if we have more number of arguments which some of them as mandatory and some of them are optional 
	// and caller no need to send optional parameters
		
	public static class Builder{
		
		//Mandatory fields
		private String type = "";
		private String name = "";
		private String userJID = "";
		private String level = "";
		
		//Optional fields
		private String descriptionText="";
		private Element operatorEl;
		private Element targetEl;
		private Element descriptionXML;
		private String status = "";
		private String target = "";
		
		
		
		public Builder(	String type,String name,String userJID,String level) {
			this.type = type;
			this.name =name;
			this.userJID=userJID;
			this.level = level;
		}
		
		public Builder status(String status){
			this.status = status;
			return this;
		}
		
		public Builder target(String target) {
			this.target = target;
			return this;
		}
		
		public Builder descriptionText(String descriptionText){
			this.descriptionText = descriptionText;
			return this;
		}
		
		public Builder operatorEl(Element operatorEL) {
			this.operatorEl = operatorEL;
			return this;
		}
		
		public Builder targetEl(Element targetEl) {
			this.targetEl = targetEl;
			return this;
		}
		
		public Builder descriptionXML(Element descriptionXML) {
			this.descriptionXML = descriptionXML;
			return this;
		}
		public AuditEvent build(){
			return new AuditEvent(this);
		}
		
	}
}
