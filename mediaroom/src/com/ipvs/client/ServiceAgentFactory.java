package com.ipvs.client;

import java.util.Hashtable;
import java.util.Iterator;

import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;

public class ServiceAgentFactory implements ServiceAgentFactoryI {

  Hashtable<String,ServiceAgentI> agents;
  Hashtable<String,Boolean> agentRegistrationStatus;
  
  public ServiceAgentFactory() {
    agents = new Hashtable<String, ServiceAgentI>();
    agentRegistrationStatus = new Hashtable<String,Boolean>();
  }
  public ServiceAgentI getServiceAgent(String name) {
    return this.agents.get(name);
  }
  public Iterator<String> getIterator() {
    return this.agents.keySet().iterator();

  }
  
  public void addAgent(String name, ServiceAgentI agent) {
    this.agents.put(name, agent);
    this.agentRegistrationStatus.put(name,Boolean.FALSE);    
  }
  
  public void removeAgent(String name) {
	this.agents.remove(name);
    this.agentRegistrationStatus.remove(name);     
  }
  
  public void setAgentStateAsReady(String name) {	  
	  this.agentRegistrationStatus.put(name, Boolean.TRUE);
  }

  public Boolean getAgentState(String name) {
	 return this.agentRegistrationStatus.get(name);
  }
}
