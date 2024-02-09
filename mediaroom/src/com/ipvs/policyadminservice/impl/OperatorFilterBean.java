package com.ipvs.policyadminservice.impl;

import java.util.List;

public class OperatorFilterBean {
	
	List<String> userFilterList = null;
	List<String> resourceFilterList = null;
	/**
	 * @return the userFilterList
	 */
	public List<String> getUserFilterList() {
		return userFilterList;
	}
	/**
	 * @param userFilterList the userFilterList to set
	 */
	public void setUserFilterList(List<String> userFilterList) {
		this.userFilterList = userFilterList;
	}
	/**
	 * @return the resourceFilterList
	 */
	public List<String> getResourceFilterList() {
		return resourceFilterList;
	}
	/**
	 * @param resourceFilterList the resourceFilterList to set
	 */
	public void setResourceFilterList(List<String> resourceFilterList) {
		this.resourceFilterList = resourceFilterList;
	}

}
