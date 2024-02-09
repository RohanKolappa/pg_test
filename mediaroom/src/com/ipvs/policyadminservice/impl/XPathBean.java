package com.ipvs.policyadminservice.impl;

import java.util.List;

public class XPathBean {
	
	private boolean isAllow = false;
	private List<StringBuffer> targetXPath = null;
	private StringBuffer objectXPath = null;
	/**
	 * @return the isAllow
	 */
	public boolean isAllow() {
		return isAllow;
	}
	/**
	 * @param isAllow the isAllow to set
	 */
	public void setAllow(boolean isAllow) {
		this.isAllow = isAllow;
	}
	/**
	 * @return the targetXPath
	 */
	public List<StringBuffer> getTargetXPath() {
		return targetXPath;
	}
	/**
	 * @param targetXPath the targetXPath to set
	 */
	public void setTargetXPath(List<StringBuffer> targetXPath) {
		this.targetXPath = targetXPath;
	}
	/**
	 * @return the objectXPath
	 */
	public StringBuffer getObjectXPath() {
		return objectXPath;
	}
	/**
	 * @param objectXPath the objectXPath to set
	 */
	public void setObjectXPath(StringBuffer objectXPath) {
		this.objectXPath = objectXPath;
	}
	

}
