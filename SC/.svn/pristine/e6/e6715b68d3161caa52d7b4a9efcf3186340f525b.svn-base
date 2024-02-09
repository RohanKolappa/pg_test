package com.barco.nimbus.utils.example.exist;

import javax.xml.namespace.QName;
import javax.xml.xquery.XQConnection;
import javax.xml.xquery.XQDataSource;
import javax.xml.xquery.XQException;
import javax.xml.xquery.XQPreparedExpression;
import javax.xml.xquery.XQResultSequence;

import net.xqj.exist.ExistXQDataSource;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;

/**
 * For this to work, you need exist to be running (localhost:8080)
 * eXist installation: http://exist-db.org/exist/apps/doc/advanced-installation.xml#headless
 */
@Component
public class ExistExample {
	
	void testExist() throws Exception {
		XQDataSource xqs = new ExistXQDataSource();
		xqs.setProperty("serverName", "localhost");
		xqs.setProperty("port", "10085");

		XQConnection conn = xqs.getConnection();

		XQPreparedExpression xqpe = conn
				.prepareExpression("declare variable $x as xs:string external; $x");

		xqpe.bindString(new QName("x"), "Hello eXist World!", null);

		XQResultSequence rs = xqpe.executeQuery();

		while (rs.next())
			Logger.getLogger(ExistExample.class).info(rs.getItemAsString(null));

		conn.close();
	}
	
	@Activate
	public void activate(){
		try {
			this.testExist();
		}
		catch(Exception e) {
			Logger.getLogger(ExistExample.class).info("ExistExample exception=" + e.toString());
		}
	}
}
