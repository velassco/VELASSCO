package velassco.datainjection.util.flume.agent;

import org.apache.flume.Event;
import org.apache.flume.EventDeliveryException;
import org.apache.flume.api.RpcClient;
import org.apache.flume.api.RpcClientFactory;
//import org.apache.flume.conf.FlumeConfiguration;
import org.apache.flume.event.EventBuilder;
import org.apache.flume.event.JSONEvent;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.SequenceFile;
import org.apache.solr.client.solrj.impl.CommonsHttpSolrServer;
import org.apache.solr.client.solrj.impl.XMLResponseParser;

import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

public class FlumeAVROManager {
	
  private static RpcClient client;
  private static String hostname;
  private static int port;	
  private static String FLUME_HOSTNAME = "flume.cimne.acuario.pez001";
  
  private static FlumeAVROManager instance = null;
  
  protected FlumeAVROManager() {
	  
  }
	  
  public FlumeAVROManager(String portId) {
	// Setup the RPC connection
	Properties properties = new Properties();
	try {
	  properties.load(this.getClass().getResourceAsStream("/flume.properties"));
	  this.hostname = properties.getProperty(FLUME_HOSTNAME);
	  //int portFlume = Integer.parseInt(properties.getProperty("flume." + portNumber));
	  String portName = "flume." + portId;
	  String portNumber = properties.getProperty(portName);
	  this.port = Integer.parseInt(portNumber);
	  this.client = RpcClientFactory.getDefaultInstance(hostname, port, 10000);
	} catch (IOException e) {
		e.printStackTrace();
	}  	  
  }
	
  // NOT STATIC !!!
//  public static FlumeAVROManager getInstance(String portNumber) {  
//      if(instance == null) {
//         instance = new FlumeAVROManager(portNumber);         
//      }
//      return instance;
//  }
	 
  public void sendEventListToFlume(List<Event> events) {
	   // Send the event
	    try {
	      System.out.println("*** BATCH SIZE: " + client.getBatchSize());	
	      client.appendBatch(events);
	    } catch (EventDeliveryException e) {
	      // clean up and recreate the client      
	    	cleanUp();
	    	client = null;
	        client = RpcClientFactory.getDefaultInstance(hostname, port, 10000);
	    } 
  }
  
  public void sendDataToFlume(byte[] data) {
    
	// Create a Flume Event object that encapsulates the sample data
    Event event = EventBuilder.withBody(data);  //withBody(data, Charset.forName("UTF-8"));

    // Send the event
    try {
      client.append(event);
    } catch (EventDeliveryException e) {
      // clean up and recreate the client      
    	cleanUp();
    	client = null;
        client = RpcClientFactory.getDefaultInstance(hostname, port, 10000);
    } 
  }

  public void cleanUp() {
    // Close the RPC connection
    client.close();
  }

  public void sendFileToFlume(File file) {
	// TODO Auto-generated method stub
			
  }

public void writeFileToHDFS(String file) {
	// TODO Auto-generated method stub
	try{
	        Path pt=new Path("hdfs:/jp./jeka.com:9000/user/jfor/out/abc");
	        FileSystem fs = FileSystem.get(new Configuration());
	        BufferedWriter br = new BufferedWriter(new OutputStreamWriter(fs.create(pt,true)));
	                                   // TO append data to a file, use fs.append(Path f)
	        br.write(file);
	        br.close();
		}catch(Exception e){
		        System.out.println("File not found");
		}
	}

//	protected Writer openWriter(String p) throws IOException {
//	  //LOG.info("Opening " + p);
//	  FlumeConfiguration conf = FlumeConfiguration.get();
//	  Path dstPath=new Path(p);
//	  FileSystem hdfs=dstPath.getFileSystem(conf);
//	  Writer w = SequenceFile.createWriter(hdfs,conf,dstPath,WriteableEventKey.class,WriteableEvent.class);
//	  return w;
//	}

}