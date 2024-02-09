package com.barco.nimbus.module.api;

import java.net.URI;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.UUID;

/**
 * A live layout, the entry point where interactive layout changes get
 * translated into incremental delta's to be distributed over peers.
 * 
 * Generates URI's that describe the channel that connects all peers of a
 * display, collaborating in rendering a session of a particular LayoutService.
 */
public interface LayoutSessionI<LR, C> {
	/**
	 * @param layout
	 *            == null allowed, meaning
	 *            "remove all content from the destination ports"
	 * @param context TODO
	 */
	void setLayout(String layer, LR layout, C context);

	/**
	 * Obtain a uri enabling one to "render" (a viewported subset of) this
	 * session. This uri is sent to the DestinationPort peer (the ngp, the
	 * decoder, ...) at DisplayService.setDisplayLayoutSession-time. The URI is
	 * not dependent on the active layout LR, only on the chosen LayoutService
	 * and the PeerProperties (eg position of the DestinationPort within the
	 * sink).
	 * @param display TODO
	 */
	List<URI> createSubscriptionURI(Display display, PeerProperties props);

	public static class PeerProperties {

		/** the logical position within the display wall */
		public final int x, y, w, h;

		/**
		 * the scaling we'll do (to compensate for heterogenous pixel size): a
		 * screen with double pixel density in comparision with the others would
		 * have 2 here, meaning that all rendering-instructions need to be
		 * stretched with factor 2.
		 */
		public final double u, v;

		/** the protocols we understand */
		public final List<String> accept;

		/** identify of the peer */
		public UUID id;

		/**
		 * The peer can express capabilities through the accept list, this
		 * is to work like http's accept header. It allows a LayoutService that
		 * supports multiple protocols, to hand out different kind of URI's to
		 * different kinds of receivers.
		 */
		public PeerProperties(UUID id,int x, int y, int w, int h, double u, double v, List<String> accept) {
			this.id = id;
			this.x = x;
			this.y = y;
			this.w = w;
			this.h = h;
			this.u = u;
			this.v = v;
			this.accept = Collections.unmodifiableList(accept);
		}
	}
}
