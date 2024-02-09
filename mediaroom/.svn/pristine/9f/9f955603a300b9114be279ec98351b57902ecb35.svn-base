package com.ipvs.utils;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Vector;

public class ProcessArguments {
	//private ArrayList<String> argList = null; 
	private Hashtable<String,String> argsParam = null; 
	private String[] argruments = null;
	ArrayList<String> keys = null;
	public ProcessArguments() {
	}
	public void addParamName(String key){
		if(keys==null)
			keys = new ArrayList<String>();
		if(key != null && !key.trim().equals("")){
			key = key.trim().toLowerCase();
			keys.add(key);
		}
	}
	public void process(String[] args ) {

		/*
		 -KeepAlive=true
		  A-'args' starts with '-' will be argsParam.
		  1> 	-Name=Value
		  2> or -Name value
		  B-Rest of the 'args' will be argList.
		  e.g.
		  java  -Djava.library.path=./lib/path/Lib 
		  -classpath .:lib/path/Lib/mylib.jar 
		  myclass 
		  $search 
		 */
		
		argsParam = new Hashtable<String,String>(); 
		argruments = new String[0];
		
		Vector<String> argList = new Vector<String> ();
		try {
			for(int i=0; i<args.length; i++){
				String arg = args[i].trim();
				if(arg.startsWith("-")){
					int iPos = arg.trim().indexOf("-");
					String key  ="";
					String value  ="";
					arg = arg.replace('-', ' ').trim();
					iPos = arg.indexOf("=");
					if(iPos != -1){
						key  = arg.substring(0,iPos);
						value  = arg.substring(iPos+1);
						addParam( argsParam, key, value);
					}else{
						key  = arg;
						if( (i+1) < args.length &&  args[i+1] != null){
							value = args[i+1].trim();
							if(!keys.contains(value) && !value.startsWith("-")) 
								i=i+1;
							 else
								value ="";
						}
						addParam( argsParam, key,value);
					}
				}else {
					if(keys.contains(arg)){
						String key  ="";
						String value  ="";
						key = arg;
						if( (i+1) < args.length &&  args[i+1] != null){
							value = args[i+1].trim();
							if(!keys.contains(value) && !value.startsWith("-")) 
								i=i+1;
							 else
								value ="";
						}
						addParam( argsParam, key,value);
					}else{
						System.out.println("argument "+arg );
						argList.add(arg);
					}
				}
			}
		} catch(Exception ex) {
			ex.printStackTrace();
		}finally{
			if(argList !=null && argList.size()>0){
				argruments = new String[argList.size()];
				argList.copyInto(argruments);
			}else if(argruments == null){
				argruments = new String[0];
			}
		}
	}
	private void addParam(Hashtable<String,String> argsParam, String key, String value){
		if(value == null || value.trim().equals(""))
			value = "";
		value =  value.trim();
		if(key != null && !key.trim().equals("")){
			argsParam.put(key.trim().toLowerCase(), value);
			System.out.println("addParam "+key.trim().toLowerCase() +":"+ value );
		}
	}
	public String[] getArgumentList() {
		if(argruments!=null) {
			return argruments.clone();
		}
		else {
			return null;
		}	
	}
	public String getArgument(int index ) {
		if(argruments!=null && argruments.length>index && argruments[index] !=null)
			return argruments[index].trim();
		return null;
	}	
	public String getArgument(int index, String sdefault) {
		if(getArgument(index) != null) 
			return getArgument(index);
		return sdefault;
	}
	public boolean getArgument(int index , boolean bdefault) {
		if(getArgument(index) !=null) 
			return Utils.getBooleanValue(getArgument(index), bdefault);
 		return bdefault ;
	}
	public int getArgumentParamInt(int index , int idefault) {
		if(getArgument(index) !=null) 
			return Utils.getIntValue(getArgument(index), idefault);
 		return idefault;
	}
	public long getArgumentParamLong(int index , long idefault) {
		if(getArgument(index) !=null) 
			return Utils.getLongValue(getArgument(index), idefault);
 		return idefault;
	}	
	///////////// params /////////////
	public Hashtable<String,String> getArgumentParams() {
		Hashtable<String,String> list = new Hashtable<String,String>();
		list.putAll(argsParam) ;
		 return list;
	}
	public String  getArgumentParam(String key) {
		if(key!=null && !key.trim().equals(""))
		{
			key=key.trim().toLowerCase();
			if(argsParam.containsKey(key)){
				String val = argsParam.get(key);
				if(val !=null)
					return val.trim();
			}
		}
		return null;
	}
	public String  getArgumentParam(String key, String sdefault) {
		String val = getArgumentParam(key);
		if(val ==null )
			return sdefault;
		if(val.trim().equals("") )
			return sdefault;
		return val.trim();
	}
	public boolean getArgumentParamBool(String key , boolean bdefault) {
		return Utils.getBooleanValue( getArgumentParam(key, bdefault+""), bdefault);
	}
	public int getArgumentParamInt(String key , int idefault) {
		return Utils.getIntValue( getArgumentParam(key, idefault+""), idefault);
	}
	public long getArgumentParamLong(String key , long idefault) {
		return Utils.getLongValue( getArgumentParam(key, idefault+""), idefault);
	}		
}
