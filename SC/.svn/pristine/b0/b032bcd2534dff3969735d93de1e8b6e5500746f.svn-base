package com.barco.nimbus.displaymanagement;

import java.net.URI;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;

import org.osgi.service.component.annotations.Component;

import com.barco.nimbus.module.api.Display;
import com.barco.nimbus.module.api.LayoutServiceI;
import com.barco.nimbus.module.api.LayoutSessionI;

/**
 * A simple implementation that points all destination ports to one (or more)
 * URL(s).
 * 
 * The interpretation of multiple url's is implementation specific; a possible
 * example using http is to pass a content url, an long-polling-update url and a
 * feedback url. A different example is n url's pointing to n application/sdp's,
 * and the endpoint is responsible for auto-layouting these.
 * 
 * Because of that lack of indirection (being that the URI's passed in are sent
 * directly to the destination ports directly), the LayoutSessionI.setLayout is
 * dummy, so there's no means to hot-switch to a different URI; you'ld need to
 * createSession(otherURI) and DisplayMgr.setLayoutSession().
 */
@Component
public class URILayoutService implements LayoutServiceI<Void, Void, Map<?, ?>> {

	private final List<URILayoutSession> sessions = new ArrayList<URILayoutSession>();

	/**
	 * context.get("uri") must be a List<URI>; these URI's must be understood by
	 * the endpoint. The destination ports will be configured with these URI's.
	 */
	@Override
	public LayoutSessionI<Void, Void> createSession(Map<?, ?> context) {
		List<URI> uri = (List<URI>) context.get("uri");
		URILayoutSession session = new URILayoutSession(uri);
		sessions.add(session);
		return session;
	}

	@Override
	public void dispose(LayoutSessionI<Void, Void> session) {
		sessions.remove(session);
	}

	@Override
	public Iterable<? extends LayoutSessionI<Void, Void>> getSessions() {
		return Collections.unmodifiableCollection(sessions);
	}

	private static class URILayoutSession implements LayoutSessionI<Void, Void> {

		private final List<URI> uri;

		public URILayoutSession(List<URI> uri) {
			this.uri = uri;
		}

		@Override
		public void setLayout(String layer, Void layout, Void context) {
		}

		@Override
		public List<URI> createSubscriptionURI(Display display, PeerProperties props) {
			List<URI> out = new ArrayList<>();
			for (URI uri : this.uri) {
				URI uri2 = URI.create(uri.toString()
						.replaceAll("`peer.viewport`", Arrays.asList(props.x, props.y, props.w, props.h).toString().replaceAll(" ", ""))
						.replaceAll("`peer.scale`", Arrays.asList(props.u, props.v).toString().replaceAll(" ", ""))
						.replaceAll("`peer.id`", Arrays.asList(props.id).toString()));
				out.add(uri2);
			}
			return out;
		}
	}

	/**
	 * This layout service has no layouts. The uri is to be passed in the
	 * context at session creation.
	 */
	@Override
	public List<Void> getLayouts() {
		return Collections.emptyList();
	}
}
