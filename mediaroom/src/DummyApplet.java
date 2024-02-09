

import java.applet.Applet;
import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.TextArea;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.lang.reflect.Method;

public class DummyApplet extends Applet implements ActionListener{
 
	private static final long serialVersionUID = 1L;
	TextArea input;
	  TextArea output;
	  Button ok;

	  public void init() {
	    setLayout(new BorderLayout());
	    input = new TextArea("input1", 2, 40);
	    add(input, "West");
	    ok = new Button("doRequest");
	    add(ok, "Center");
	    output = new TextArea("output", 2, 40);
	    add(output, "East");
	    ok.addActionListener(this);
	  }

	  public String login(String user, String password) {
		  return "2"; // TBD return a error code
	  }
	  
	  public String logout() {
		  return "1";
	  }
	  
	  public String sendRequest(String message) {
	    input.setText("request=" + message);
	    return "requestNID";
	  }

	  public void handleResult() {
	    String message = input.getText();
	    String result = message + " result";
	    output.setText(result);
	    try {
	      callJSMethod("parent.main.handleResult", result);
	    }
	    catch(Exception e) {
	      e.printStackTrace();
	    }
	  }

	  private void callJSMethod(String methodName, String methodArgs) throws Exception {
	    String jscmd = methodName + "('" + methodArgs + "');";
	    output.setText(jscmd);
	    Method getw = null, eval = null;
	    Object jswin = null;
	    Class<?> c = Class.forName("netscape.javascript.JSObject");
	    Method ms[] = c.getMethods();
	    for(int i = 0; i < ms.length; i++) {
	      if(ms[i].getName().compareTo("getWindow") == 0)
	        getw = ms[i];
	      else if(ms[i].getName().compareTo("eval") == 0)
	        eval = ms[i];
	    }
	    Object a[] = new Object[1];
	    a[0] = this;
	    jswin = getw.invoke(c, a); // Yields the JSObject
	    a[0] = jscmd;
	    //Object result = 
	    eval.invoke(jswin, a);
	  }

	  public void actionPerformed(ActionEvent e) {
	    if(e.getSource() == ok) {
	      handleResult();
	    }
	  }

}
