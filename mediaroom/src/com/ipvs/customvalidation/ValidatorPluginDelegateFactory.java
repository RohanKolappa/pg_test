package com.ipvs.customvalidation;
 
import java.util.Hashtable;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.common.MRException;
public class ValidatorPluginDelegateFactory
{
	public static final String COMMON_VALIDATOR_PLUGIN= "com.ipvs.customvalidation.CommonValidatorPlugin";
	private static ValidatorPluginDelegateFactory customValidatorFactory=null;
	private ValidatorPluginDelegateFactory(){
		listDataModel = new Hashtable<String, ValidatorPluginDelegate>();
	}
	public static ValidatorPluginDelegateFactory getInstance(){
		if(customValidatorFactory==null)
			customValidatorFactory = new ValidatorPluginDelegateFactory();
		return customValidatorFactory;
	}
	
	private Hashtable<String, ValidatorPluginDelegate> listDataModel =null;
 	public  ValidatorPluginDelegate  getValidatorPluginDelegate(String forClass) throws Exception{
		if(listDataModel==null)
			listDataModel = new Hashtable<String, ValidatorPluginDelegate>();
		if(forClass==null || forClass.equals(""))
			forClass = COMMON_VALIDATOR_PLUGIN;
		if(!listDataModel.containsKey(forClass)){
			Object obj= getClassInstace(forClass);
			if(obj==null){
				throw new MRException(AMExceptionCodes.OBJECT_DOES_NOT_EXIST,"<error>"+"Object does not exist"+"</error>");
			}
			ValidatorPluginDelegate customValidator = new ValidatorPluginDelegate();
			customValidator.setObject(obj); 
			listDataModel.put(forClass, customValidator );
		}
		return listDataModel.get(forClass);
	}
	private Object getClassInstace(String handlerName) {
 		try {
			Class<?> c = Class.forName(handlerName);
			return c.newInstance();
		}
		catch(Exception e) {
		}
		return null;
	}
  
  
}



/*
 
   
import java.lang.reflect.*;
public class Test
{
  public static void main(String[] args)
  {
    try
    {
      Class c = Class.forName("Foo");
      System.out.println("Loaded class: " + c);
      Method m = c.getDeclaredMethod("getNum", null);
      System.out.println("Got method: " + m);
      Object o = m.invoke(null, null);
      System.out.println("Output: " + o);
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }
}
class Foo
{
    public static int getNum()
    {
      return 5;
    }
} 



import java.lang.reflect.Method;

// Test return values of Method.invoke.
public class InvokeReturn {
  public boolean bTrue() {
    return true;
  }

  public boolean bFalse() {
    return false;
  }

  public char cc() {
    return 'c';
  }

  public short s5() {
    return (short)5;
  }

  public int i6() {
    return 6;
  }

  public long l7() {
    return (long)7;
  }

  public float f8() {
    return (float)8.0;
  }

  public double d9() {
    return 9.0;
  }

  public static void main(String[] args) {
    try {
      Object o = new InvokeReturn();
      Method m;

      // test boolean result
      m = o.getClass().getDeclaredMethod("bTrue", new Class[0]);
      System.out.println(m.invoke(o, new Object[0]));

      m = o.getClass().getDeclaredMethod("bFalse", new Class[0]);
      System.out.println(m.invoke(o, new Object[0]));

      // test char result
      m = o.getClass().getDeclaredMethod("cc", new Class[0]);
      System.out.println(m.invoke(o, new Object[0]));

      // test short result
      m = o.getClass().getDeclaredMethod("s5", new Class[0]);
      System.out.println(m.invoke(o, new Object[0]));

      // test int result
      m = o.getClass().getDeclaredMethod("i6", new Class[0]);
      System.out.println(m.invoke(o, new Object[0]));

      // test long result
      m = o.getClass().getDeclaredMethod("l7", new Class[0]);
      System.out.println(m.invoke(o, new Object[0]));

      // test float result
      m = o.getClass().getDeclaredMethod("f8", new Class[0]);
      System.out.println(m.invoke(o, new Object[0]));

      // test double result
      m = o.getClass().getDeclaredMethod("d9", new Class[0]);
      System.out.println(m.invoke(o, new Object[0]));
    } catch (Throwable t) {
      t.printStackTrace();
    }
  }
}

 */