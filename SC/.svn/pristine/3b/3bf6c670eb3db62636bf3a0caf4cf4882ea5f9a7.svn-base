package com.barco.nimbus.displaymanagement.bms;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.QueryParam;

import org.jboss.resteasy.annotations.cache.Cache;

@Path("NetworkWall")
public interface BMSSessionServiceI {
	@GET
	@Path("/session/{display}")
	@Cache(mustRevalidate=true)
	Iterable<?> getDisplayContents(@PathParam("display") int display, @QueryParam("token") String token);

}
