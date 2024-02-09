package com.ipvs.customvalidation;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Hashtable;

import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;

public class ValidatorPluginDelegate {
	private Object obj=null;
	private  Hashtable<String, Method> methods =null; 
 
	public void setObject(Object obj){
		this.obj=obj;
	}
	public Object getObject(){
		 return obj;
	}
	public void listMethods() throws Exception {
		// TODO Auto-generated method stub
		  methods = new Hashtable<String, Method>();
		  Method [] methodArr =obj.getClass().getDeclaredMethods();
		  for(int i=0;i<methodArr.length;i++){
			  Method method= methodArr[i];
			  methods.put(method.getName(), method);
		  }
	}
 
	public Object invoke(String methodName, Object[] params) throws Exception {
		if(obj==null){
			throw new MRException(AMExceptionCodes.OBJECT_DOES_NOT_EXIST,"<error>"+"Object not set"+"</error>"); //TODO:
		}
		if(methods==null){
			listMethods(); 
		}
		Method method = methods.get(methodName);
	 
		if(method==null){
			throw new MRException(AMExceptionCodes.METHOD_DOES_NOT_EXIST,"<error>"+"method not declared"+"</error>"); //TODO:
		}
		try{
		    return method.invoke(obj, params);
        }catch(InvocationTargetException ex){
            Object obj = ex.getTargetException();
            if(obj==null){
                throw new Exception("Validation error ");
            }
            if(obj instanceof MRException){
                throw (MRException)obj;
            }else if(obj instanceof Exception){
                throw (Exception)obj;
            }else if(obj instanceof Throwable){
                String mesg = ex.getTargetException().getMessage();
                throw new Exception("Validation error "+ mesg);
            }
        }
        return false;
 	}

	 
	public boolean validateEL( String action, String serviceName,  String dbObjectName, Element elReq, Element elCustomValidate,
			MRRequest request, XMPPI xmpp, MRLog log, MRDB schema,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		 
		String methodName = MRXMLUtils.getAttributeValue(elCustomValidate, "methodName", "").trim();
		if(methodName.equals(""))
			return true;
		Object [] params = {action, serviceName, dbObjectName, elReq, elCustomValidate, request, xmpp, log, schema,
				resourceFactory, serviceAgentFactory};
		Object ret = invoke(methodName,params);
		if(ret instanceof Boolean) {
			return ((Boolean)ret).booleanValue();
		} 
		if(ret ==null) {
			return false;
		} 
		return true;
	}
	 
 
}
