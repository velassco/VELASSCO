package velassco.datainjection.util;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.net.ServerSocket;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.StringTokenizer;

import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;

import org.apache.flume.Event;
import org.apache.flume.EventDeliveryException;
import org.apache.flume.event.EventBuilder;

import velassco.datainjection.bean.MessageBean;

import com.sun.jersey.api.client.Client;
import com.sun.jersey.api.client.WebResource;
import com.sun.jersey.api.client.config.DefaultClientConfig;

/**
 * Data Injector Manager is in charge of handling the Data Injector processes receiving the input file 
 * and assigning every portion into uniques endpoints and ports  
 *
 */

public class DataInjectorManager {
	
	private static DataInjectorManager instance = null;
	
	public static DataInjectorManager getInstance() {
	      if(instance == null) {
	         instance = new DataInjectorManager();         
	      }
	      return instance;
	  }
	
	/* ASIGNAR PUERTO */
    // If you don't mind the port used, specify a port of 0 to the ServerSocket constructor and it will listen on any free port.
	private ServerSocket create(int[] ports) throws IOException{
		ServerSocket s = new ServerSocket(0);
		System.out.println("listening on port: " + s.getLocalPort());
		return s;
	}
	
    // If you want to use a specific set of ports, then the easiest way is probably to iterate through them until one works. Something like this:
    public int seekFreePort() throws IOException {
    	ServerSocket socket = null;
    	try {
    		socket = create(new int[] { 61616, 61617, 61618});
            System.out.println("listening on port: " + socket.getLocalPort());
        } catch (IOException ex) {
            System.err.println("no available ports");
        }
        return socket.getLocalPort();
    }	
	
	public String splitInputFile(){
    	// TO-DO
    	return "";
    }
    
}
