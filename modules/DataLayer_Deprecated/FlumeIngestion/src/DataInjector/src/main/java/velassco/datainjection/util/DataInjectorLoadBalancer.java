package velassco.datainjection.util;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import org.apache.flume.Event;

public class DataInjectorLoadBalancer {
	
	@Path("manager/balacerLoad")
    @GET
	@Produces("application/xml")
	public void balacerLoad(Event event){
		sendData(event);
	}

	private void sendData(Event event) {
		// TODO Auto-generated method stub
		
	}

}
