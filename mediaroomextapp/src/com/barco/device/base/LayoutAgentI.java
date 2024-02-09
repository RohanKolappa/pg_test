package com.barco.device.base;

import org.dom4j.Document;

public interface LayoutAgentI extends AgentI{

	// When acting as a Destination
	public abstract void updateDisplayConfig(Document displayConfig);
	public abstract Document getDisplayConfig();

}
