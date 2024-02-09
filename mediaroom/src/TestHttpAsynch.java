import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.core.Response;

import org.jboss.resteasy.annotations.Suspend;
import org.jboss.resteasy.spi.AsynchronousResponse;

@Path("test")

public class TestHttpAsynch {
	int counter =1;
		@Path("async")
		@GET
		public void doAsynGet(final @Suspend(1000) AsynchronousResponse asynResponse) {
			System.out.println("testing " + counter++);
			try {
				Thread.sleep(5000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			asynResponse.setResponse(Response.status(HttpServletResponse.SC_OK).entity("Response").build());
			System.out.println("Returned Response");			
		}
		
		
		@GET
		public Response doGet() {
			System.out.println("testing " + counter++);
			try {
				Thread.sleep(5000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			System.out.println("Return Response ");
			
			return Response.status(HttpServletResponse.SC_OK).entity("Response").build();
			
		}
}
