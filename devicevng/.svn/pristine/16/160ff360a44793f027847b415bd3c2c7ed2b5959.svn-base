package com.barco.device.vng.utils;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketException;
import java.util.Properties;
import java.util.Vector;

import org.apache.log4j.Logger;
import org.xmlpull.mxp1.MXParser;
import org.xmlpull.v1.XmlPullParser;
/*
class Logger {
	void info(String s){ System.out.println(s);}
	void error(String s){ System.out.println(s);}
}
*/

class XMLParser  {

	/**
	 * @return the next node (as xml-string) from a pull-parser.
	 */
	public String parse(XmlPullParser parser) throws Exception {
		
		if (parser.getEventType() == XmlPullParser.TEXT)               
			return parser.getText();
			
		StringBuffer xml = new StringBuffer();
		String rootName = parser.getName();
		boolean emptyTag = false;
		while(true) {
			// TBD test and handle single tag in IQ
			// TBD test and handle xml control chars in IQ
			emptyTag = this.consumeToken(parser, xml, emptyTag);
			if(		XmlPullParser.END_TAG == parser.getEventType() && 
					rootName.equals(parser.getName()))
				break;
				parser.next();
		}
		return xml.toString();
	}
	
	private boolean consumeToken(XmlPullParser parser, StringBuffer xml, boolean emptyTag) throws Exception{
		if (parser.getEventType() == XmlPullParser.START_TAG) {
			xml.append("<").append(parser.getName());  
			int n = parser.getAttributeCount();    
			for (int i = 0; i < n; i++) {
				xml.append(" ").append(         
						parser.getAttributeName(i)).append("=\"") 
						.append(escapeXMLText(parser.getAttributeValue(i))) 
						.append("\"");               
			}                                 
			if (parser.isEmptyElementTag()) {    
				xml.append("/>");        
				emptyTag = true;              
			} else                           
				xml.append(">");     
		} else if (parser.getEventType() == XmlPullParser.END_TAG) {
			if (emptyTag)            
				emptyTag = false;     
			else                      
				xml.append("</").append(parser.getName()).append(">");             
		} else if (parser.getEventType() == XmlPullParser.TEXT) {                 
				xml.append(escapeXMLText(parser.getText()));                             
		}  
		return emptyTag;
	}
	
	private String escapeXMLText(String textValue){
		textValue = textValue.replaceAll("&", "&amp;");
		return textValue.replaceAll("<", "&lt;");
	}

	public static Properties parseProperties(XmlPullParser parser) throws Exception {
		parser.nextToken();
		int n = parser.getAttributeCount();
		Properties p = new Properties();
		for (int i = 0; i < n; i++) {     
			p.setProperty(parser.getAttributeName(i), parser.getAttributeValue(i)); 
		}
		return p;
	}

}

/**
 * The thread pulls out xml from a reader, and delivers it as a series of
 * Strings (getNextXML) containing each a single xml element.
 */
class XMLParserThread extends Thread {
	boolean done;
	boolean resetRequired;
	BufferedReader reader;
	XmlPullParser parser;
	XMLParser xmlParser;
	Logger log;
	Vector<String> queue;
	PipedOutputStream pipeSource = null;
	PipedInputStream pipeSink = null;
	PrintWriter pipeWriter = null;
	char [] buffer = new char[1024];
	String rootElementStr = null;
	
	XMLParserThread(BufferedReader reader, Logger log) {
		this.reader = reader;
		this.done = false;
		this.queue = new Vector<String>();
		this.log = log;
		this.parser = new MXParser();
		this.xmlParser = new XMLParser();
	}
	
	void init() throws Exception {
		pipeSource= new PipedOutputStream();
		pipeSink = new PipedInputStream(pipeSource);
		pipeWriter = new PrintWriter(pipeSource);
		parser.setInput(new BufferedReader(new InputStreamReader(pipeSink)));
		this.rootElementStr = reader.readLine();
		System.out.println("XMLStreamReaderThread:init read=" + rootElementStr);
		pipeWriter.write(rootElementStr);
		parser.next();
		this.done = false;
	}
	
	synchronized void add(String s) {
		// log.info("XMLParserThread:add ..."  );
		this.queue.add(s);
		// log.info("XMLParserThread:add done"  );
	}
	
	synchronized String next() throws Exception {
		if(this.queue.isEmpty())
			return null;
		log.info("XMLParserThread:getNextXML read=" + this.queue.size());
    	return this.queue.remove(0);
	}

	String getNextXML() throws Exception {
		// log.info("XMLParserThread:getNextXML waiting");
		String str = this.next();
		if(str!=null){
			return str;
		}
		
		
		do{			
			int bytesRead = reader.read(buffer);
			
			if(bytesRead == -1) {
				resetRequired = true;
				log.error("Reader got -1 server might be restarted. Reconnect to the server socket");
				throw new SocketException("Socet disconnected restart the service");				
			}
			
			pipeWriter.write(buffer, 0, bytesRead);
			pipeWriter.flush();
			//log.info("==========XMLParserThread:getNextXML read=" + String.valueOf(buffer));
		} while(reader.ready());
		
		/*while(reader.ready()) {
			int bytesRead = reader.read(buffer);
			pipeWriter.write(buffer, 0, bytesRead);
			pipeWriter.flush();
			log.info("XMLParserThread:getNextXML read=" + String.valueOf(buffer));
		}*/
		
    	return this.next();
	}

	void setDone() {
		log.info("XMLParserThread:setDone");
		this.done = true;
		pipeWriter.write("</" + XMLParserThread.class.getName() + ">\n");
		pipeWriter.flush();
	}

	public void run() {
		try {
			this.init();
		}
		catch(Exception e){
			log.error("XMLParserThread:run init error= " + e.toString());
			this.done = true;
		}
		while(!this.done) {
			try {
				parser.next(); // Will wait here until the next start token is available
				String s = xmlParser.parse(parser);
				if((s != null) && (!s.trim().isEmpty())) {
					// log.info("XMLParserThread:run token=" + s  );
					this.add(s);
					// log.info("XMLParserThread:run done=" + done  );
				}
			}
			catch(Exception e) {
				//e.printStackTrace();
				log.error("XMLParserThread:xmlParser error= " + e.toString());
			}
		}
		log.info("XMLParserThread:run exited"  );
	}
}

/**
 * This thread spawns another thread to read xml from an input stream, and
 * delivers xml-strings (containing a single element) to the handleXML(xml)
 * template method (meant to be overridden).
 */
public class XMLStreamReaderThread extends Thread {
	boolean done;
	public boolean resetRequired;
	InputStream input;
	BufferedReader reader;
	PrintWriter writer;
	Vector<String> xmlQueue;
	XMLParserThread xmlParserThread;
	Logger log;
	OutputStream output;
	
	public XMLStreamReaderThread(Logger log, InputStream input, OutputStream output) {
		this.log = log;
		this.input = input;
		this.output = output;
	}
	
	public void init() throws Exception {
		this.reader = new BufferedReader(new InputStreamReader(input));
		this.writer = new PrintWriter(output);
		this.xmlParserThread = new XMLParserThread(reader, log);
		this.done = false;
		this.resetRequired=false;
	}
	
	protected void handleXML(String s) {
		log.info("XMLStreamReader:handleXML=" + s  );
	}
	
	public void setDone() {
		log.info("XMLStramReaderThread: SetDone true");
		this.done = true;
	}
	
	public void run() {
		try {
			this.init();
			this.xmlParserThread.start();
		}
		catch(Exception e){
			log.error("XMLStreamReader:run init error= " + e.toString());
			this.done = true;
		}
		while(!this.done) {
			if(this.xmlParserThread == null)
				break;
			try {
				Thread.sleep(1000); // Used to implement the timeout
				while(true) {
					String xml = this.xmlParserThread.getNextXML();
					if(xml != null) {
						this.handleXML(xml);
					} else {
						break;
					}
				}
			}
			catch(SocketException se) {
				log.error("XMLStreamReader:run client connection error" + se.toString());
				resetRequired=true;
				break;
			} catch(org.xml.sax.SAXParseException exp) {
				log.error("XMLStreamReader:run client connection error" + exp.toString());
				resetRequired=true;
				break;
			}
			catch(Exception e) {
				//e.printStackTrace();
				log.error("XMLStreamReader:run handleXML Failed :" + e.toString());
			}
		}

		// Stop the readerThread
		if(this.xmlParserThread != null)
			this.xmlParserThread.setDone();

		// Clean up the client connection
		try {
			input.close();
			output.close();
		}
		catch(Exception e) {
			//e.printStackTrace();
			log.error("XMLStreamReader:Exception while closing input/putput stream: "+ e.toString());
		}
		log.info("XMLStreamReader:run closed client connection");
	}
			
	public static void main(String[] args) throws Exception {
		Logger log = null;
		// log = new Logger();
    	// XMLStreamReaderThread streamReader = new XMLStreamReaderThread(log, System.in, System.out);
		Socket socket = new Socket("10.51.52.52", 10002);
    	XMLStreamReaderThread streamReaderThread = new XMLStreamReaderThread(log, socket.getInputStream(), System.out);
		streamReaderThread.start();
		streamReaderThread.join();
	}    
}
