package com.barco.device.vng.utils;

import org.dom4j.Element;

public class MediaTimelineUtils {

	public static void updateMediaTimelineElement(Element mediaTimelineElement,
			long WC, long offset, long tc, double timescale, String action, String lastState, String requestedAction, String sourceAction) throws Exception {
		Element last = mediaTimelineElement.element("Last");
		last.addAttribute("WC", String.valueOf(WC));
		last.addAttribute("TimeScale", String.valueOf(timescale));
		last.addAttribute("TC", String.valueOf(tc));
		last.addAttribute("Offset", String.valueOf(offset));
		last.addAttribute("lastState", lastState);
		last.addAttribute("action", action);
		last.addAttribute("requestedAction", requestedAction);
		last.addAttribute("sourceAction", sourceAction);
		last.addAttribute("ssTC", "");
		last.addAttribute("ssWC", "");
	}

}
