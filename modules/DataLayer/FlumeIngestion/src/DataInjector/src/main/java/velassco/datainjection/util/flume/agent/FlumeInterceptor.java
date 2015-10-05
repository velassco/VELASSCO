package velassco.datainjection.util.flume.agent;

import java.util.Iterator;
import java.util.List;

import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.interceptor.Interceptor;

import java.util.Map;

import org.codehaus.jackson.JsonNode;
import org.codehaus.jackson.JsonProcessingException;
import org.codehaus.jackson.map.ObjectMapper;;

public class FlumeInterceptor implements Interceptor {

	private final String _header = "location";
	private final String _defaultLocation = "US";

	public Event intercept(Event event) {
	     String location = _defaultLocation;
	     
	     byte[] eventBody = event.getBody();

	     try {
	        ObjectMapper mapper = new ObjectMapper();
	        JsonNode rootNode = mapper.readTree(new String(eventBody));		
	        location = rootNode.get("location").toString();	
	     } catch (JsonProcessingException e) {
			
	     } catch (Exception e) {
		
	     }
			
	     Map headers = event.getHeaders();
	     headers.put(_header, location);
			
	     event.setHeaders(headers);
			
	     return event;
	  }

	public List<Event> intercept(List<Event> arg0) {
		// TODO Auto-generated method stub
		return null;
	}

	public void close() {
		// TODO Auto-generated method stub
		
	}

	public void initialize() {
		// TODO Auto-generated method stub
		
	}
}