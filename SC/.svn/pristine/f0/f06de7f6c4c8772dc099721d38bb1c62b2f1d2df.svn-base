package com.barco.nimbus.module.api;

import java.net.URI;
import java.util.List;

/**
 * maps to http://sati.barco.com/asciidoc engine document.
 * 
 * A destination port (Screen) is configured with a Content when it is
 * associated with a Display. This enables a destination port to setup
 * communication with the particular LayoutService of that display, in order to
 * obtain the layout description and updates.
 * 
 * A Content is a list of URI's. Each uri somehow identifies content to be
 * shown: eg a html file, an rtsp uri, an inline sdp.
 * 
 * If multiple uri's are present, the thing must try to show them all, eg
 * generating a grid, stacking, muxing, ... this is device-dependent (and may
 * depend· on other device-config, like the "ipvs-layouts").
 * 
 * An sdp can carry multiple m='s exposing the stream over multiple qualities
 * (eg rtp full quality and http-streaming x-mixed-replace thumbnail, a 'left'
 * part and a 'right' part, ...). It is the decision of the device to pick or
 * combine the appropriate parts. The decision can be influenced by other device
 * config (eg stating that the device is the "left" one, only meant to show
 * "left" things).
 * 
 * Not all implementations can handle all kinds or uri's, that is a matter of
 * capabilities. Example: nexxis dongles only understand "*.html",· BMS NGP's
 * only understand "jgroups://..."
 */
public class Content {
	public final String name;
	public final List<URI> sources;

	public Content(String name, List<URI> uri) {
		this.name = name;
		this.sources = uri;
	}
}