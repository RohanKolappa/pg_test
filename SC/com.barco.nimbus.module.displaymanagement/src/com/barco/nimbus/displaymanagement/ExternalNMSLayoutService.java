package com.barco.nimbus.displaymanagement;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;

import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.auth.AuthScope;
import org.apache.http.auth.UsernamePasswordCredentials;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.osgi.service.component.annotations.Component;

import com.barco.nimbus.module.api.Display;
import com.barco.nimbus.module.api.LayoutServiceI;
import com.barco.nimbus.module.api.LayoutSessionI;

/**
 * A simple implementation that depends on an external NMS server.
 */
@Component
public class ExternalNMSLayoutService implements LayoutServiceI<String, URI, Map<?, ?>> {

	private final List<String> layouts = Arrays.asList("/Default/full.layout", "/Default/pip.layout", "/Default/pap.layout", "/Default/empty.layout",
			"/Default/external.layout");
	private final List<NMSLayoutSession> sessions = new ArrayList<NMSLayoutSession>();

	/**
	 * Context contains an int "id", String "sink" and a URI "host" that
	 * identify the display-session within the NMS.
	 */
	@Override
	public LayoutSessionI<String, URI> createSession(Map<?, ?> context) {
		int id = (Integer) context.get("id");
		URI host = (URI) context.get("host");
		String sink = (String) context.get("sink");
		NMSLayoutSession session = new NMSLayoutSession(id, host, sink);
		sessions.add(session);
		return session;
	}

	@Override
	public void dispose(LayoutSessionI<String, URI> session) {
		sessions.remove(session);
	}

	@Override
	public Iterable<? extends LayoutSessionI<String, URI>> getSessions() {
		return Collections.unmodifiableCollection(sessions);
	}

	private static class NMSLayoutSession implements LayoutSessionI<String, URI> {

		private final int id;
		private final URI host;
		private final String sink;

		public NMSLayoutSession(int id, URI host, String sink) {
			this.id = id;
			this.host = host;
			this.sink = sink;
		}

		/**
		 * This implementation uses NMS api to activate the layout.
		 * 
		 * @param layout
		 *            is one of full, pip, pap, emtpy, external,
		 * @param url
		 *            points to an external html (and is only used if layout ==
		 *            "external")
		 */
		@Override
		public void setLayout(String layer, String layout, URI url) {
			List<? extends NameValuePair> p = Arrays.asList(new BasicNameValuePair("sink", sink), new BasicNameValuePair("layout", layout),
					new BasicNameValuePair("url", url.toString()));
			URI uri = URI.create(host + "/barco-webservice/rest/NetworkWall/session");
			try {
				HttpPost post = new HttpPost(uri);
				post.setEntity(new UrlEncodedFormEntity(p, "UTF-8"));
				String user = "admin";
				String pass = "admin";
				DefaultHttpClient c = new DefaultHttpClient();
				c.getCredentialsProvider().setCredentials(new AuthScope(host.getHost(), host.getPort()), new UsernamePasswordCredentials(user, pass));
				HttpResponse response = c.execute(post);
				int status = response.getStatusLine().getStatusCode();
				if (status != 204 && status != 200) {
					throw new RuntimeException("POST " + uri + post.getEntity().toString() + " gives " + response.getStatusLine());
				}
			} catch (UnsupportedEncodingException e) {
				throw new RuntimeException(e);
			} catch (ClientProtocolException e) {
				throw new RuntimeException(e);
			} catch (IOException e) {
				throw new RuntimeException(e);
			}
		}

		/**
		 * Nexxis decoder dongles understand (per display) a list of 3 urls,
		 * meaning [ state-url, long-polling-url, feedback-url]. The associated
		 * display will show the html GET from state-url, refresh that whenever
		 * GET long-poll says status == 201 or ==200, and POST information about
		 * what it's currently doing to feedback-url
		 */
		@Override
		public List<URI> createSubscriptionURI(Display display, PeerProperties props) {
			return Arrays.asList(URI.create(host + "/barco-webservice/rest/NetworkWall/session/" + id),
					URI.create(host + "/barco-webservice/rest/NetworkWall/event/session3?timeoutMs=5000&selector-session=" + id),
					URI.create(host + "/barco-webservice/rest/lan/feedback/device/" + props.id + "/?display=" + id));
		}
	}

	/**
	 * This layout service has no layouts. The uri is to be passed in the
	 * context at session creation.
	 */
	@Override
	public List<String> getLayouts() {
		return Collections.unmodifiableList(layouts);
	}
}
