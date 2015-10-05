package velassco.datainjection.util.flume.agent;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.flume.Event;
import org.apache.flume.EventDeliveryException;
//import org.apache.flume.agent.embedded.EmbeddedAgent;


public class FlumeHBaseAgent {
	
//	private EmbeddedAgent agent; // NO static so far, different agents tro be created!!!
//	
//	private void createAgent(String port, String tableName, String columnFamily) {
//		
////		https://flume.apache.org/FlumeDeveloperGuide.html#embedded-agent	
//		Map<String, String> properties = new HashMap<String, String>();
//		properties.put("channel.type", "memory");
//		properties.put("channel.capacity", "100000");
//		properties.put("channel.transactionCapacity", "1000");
//		properties.put("sinks", "hbaseSink");
//		// HBaseSink
//		properties.put("hbaseSink.hostname", "localhost"); //khresmoi3.atosresearch.eu
//		properties.put("hbaseSink.port", port);
//		properties.put("hbaseSink.type", "HBaseSink"); // only avro is supported
//		properties.put("hbaseSink.channel","memoryChannel");
//		properties.put("hbaseSink.table", tableName);
//		properties.put("hbaseSink.columnFamily", columnFamily);		
//		properties.put("hbaseSink.serializer","org.apache.flume.sink.hbase.RegexHbaseEventSerializer");
////		properties.put("hbaseSink.serializer.regex","(.+)-(.+)-(.+)$");
////		properties.put("hbaseSink.serializer.colNames","simulId,partResult,raw_data");			
//		properties.put("processor.type", "failover"); //load_balance requires group
//		
//		try {
//			agent = new EmbeddedAgent("agentHBase");
//			agent.configure(properties);
//			agent.start();
//		} catch (final Exception ex) {
//			System.err.println(ex.toString());
//			}
//		}
//	
//	public EmbeddedAgent getFlumeAgent(int port, String tn, String cf) {
//		//if (agent == null) {
//		createAgent(Integer.toString(port), tn, cf);
//		//}
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
//	
//	public void putAll(List<Event> events) throws EventDeliveryException{
//		agent.putAll(events);
//	}
}	