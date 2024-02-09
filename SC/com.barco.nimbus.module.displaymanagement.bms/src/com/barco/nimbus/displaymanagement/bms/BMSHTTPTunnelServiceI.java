package com.barco.nimbus.displaymanagement.bms;

import java.util.List;

import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;

@Path("")
public interface BMSHTTPTunnelServiceI {
	@Path("/local/{name:.*}/{method}")
	@POST
	@GET
	@Consumes("application/x-java-serialized-object")
	@Produces("application/x-java-serialized-object")
	public Object EJBInvocation(@PathParam("name") String name, @PathParam("method") String method_, @QueryParam("arg") List<String> args, Object body) throws Exception;

}
