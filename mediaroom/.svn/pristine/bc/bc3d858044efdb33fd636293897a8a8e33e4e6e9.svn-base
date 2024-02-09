package com.ipvs.config;
import java.io.File;
import java.util.Properties;

import com.ipvs.utils.Utils;
public class IPVSConfig {
	//properties file
	private Properties props = null;
	public static final String propFilePath="/com/ipvs/config/mediaroom.properties";
	private static IPVSConfig ipvsConfig= null; 
	public static synchronized IPVSConfig getInstance(){
		if(ipvsConfig==null){
			ipvsConfig= new IPVSConfig();
			//TODO: read xml path from openfire Global settings
			ipvsConfig.loadProperties(propFilePath);
		}
		return ipvsConfig;
	}
	public IPVSConfig(){
		props = new Properties();
	}
 	public void loadProperties(String propPath) {
		//propPath = getIPVSPropPath(  propPath);
		props = new Properties();
		try{
			if( ApplicationContext.isAppletContext()){
				try{
					props = Utils.loadPropertiesResource(props, propPath, IPVSConfig.class);
				}catch(Exception ex) {
					System.out.println("ERROR propPath not found ='" +propPath +"'");
				}
			} else {
				File file = new File(propPath);
				if(file.exists()){
					props = Utils.loadPropertiesFile(props, propPath, IPVSConfig.class);
				}else{
					props = Utils.loadPropertiesResource(props, propPath, IPVSConfig.class);
				}
			}
		}catch(Exception ex){
			System.out.println("ERROR propPath not found ='" +propPath +"'");
			ex.printStackTrace();
		}
	}
	public String getProperty(String propName , String sdefault) {
		if(sdefault !=null)
			return props.getProperty(propName , sdefault).trim();
		String val  = props.getProperty(propName , "").trim();
		if(!val.equals(""))
			return val ;
		return sdefault;
	}
	public boolean getPropertyBool(String propName , boolean bdefault) {
		String propValue = getProperty(propName , bdefault +"");
		if(propValue.equals("true") || propValue.equals("1")  )
			return true;
		return bdefault ;
	}
	public int getPropertyInt(String propName , int idefault) {
		String propValue = getProperty(propName , idefault+"");
		try {
			return Integer.parseInt(propValue);
		}catch(Exception ex){
		}
		return idefault;
	}
	public long getPropertyLong(String propName , long idefault) {
		String propValue = getProperty(propName , idefault+"");
		try {
			return Long.parseLong(propValue);
		}catch(Exception ex){
		}
		return idefault;
	}	
	public boolean isInited() {
		return  getPropertyBool("props.inited",  false ); 
	}
	public Properties getProps() {
 		return props;
	}
 	
}
