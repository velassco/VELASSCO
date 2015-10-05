package velassco.datainjection.util.flume.agent;

import java.util.HashMap;
import java.util.Map;

//import org.apache.flume.agent.embedded.EmbeddedAgent;


public class FlumeHDFSAgent {
	
//	private static EmbeddedAgent agent;
//	
//	private void createAgent(String port) {
//		final Map<String, String> properties = new HashMap<String, String>();
//		properties.put("channel.type", "memory");
//		properties.put("channel.capacity", "100000");
//		properties.put("channel.transactionCapacity", "1000");
//		properties.put("sinks", "HDFSSink");
//		// HDFS Sink
//		properties.put("HDFSSink.hostname", "localhost");
//		properties.put("HDFSSink.port", port);
//		properties.put("HDFSSink.type", "hdfs");
//		properties.put("HDFSSink.channel", "MemChannel");
//		properties.put("HDFSSink.hdfs.path", "hdfs://192.168.215.61:9000/home/hdfs/velassco");
//		properties.put("HDFSSink.hdfs.fileType","DataStream");
//		properties.put("HDFSSink.hdfs.writeFormat","Text");
//		properties.put("HDFSSink.hdfs.batchSize","1000");
//		properties.put("HDFSSink.hdfs.rollSize","0");
//		properties.put("HDFSSink.hdfs.rollCount","10000");
//				
//		properties.put("processor.type", "default");
//		try {
//			agent = new EmbeddedAgent("myagent");
//			agent.configure(properties);
//			agent.start();
//		} catch (final Exception ex) {
//			System.err.println(ex.toString());
//			}
//		}
//	
//	public EmbeddedAgent getFlumeAgent(int port) {
//		if (agent == null) {
//			createAgent(Integer.toString(port));
//		}
//		return agent;
//	}
//	
//	public String stopAgent(){
//		String msg = "Stopping agent...";
//		try {
//			agent.stop();
//		} catch (final Exception ex) {
//			System.err.println(ex.toString());
//			msg += "Error stopping agent: " + ex.toString();
//		}
//		return msg;
//	}
}
