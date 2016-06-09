package velassco.datainjection.resources;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.io.StringWriter;
import java.net.ServerSocket;
import java.net.URI;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Random;
import java.util.StringTokenizer;
import java.util.UUID;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
import javax.ws.rs.DefaultValue;
import javax.ws.rs.FormParam;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.PUT;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.MediaType;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.transform.stream.StreamSource;

import net.sf.json.JSONException;

import org.apache.solr.client.solrj.SolrServerException;
import org.apache.xerces.dom.events.EventImpl;

import com.google.gson.Gson;

import org.apache.commons.codec.binary.Base64;
import org.apache.commons.codec.binary.StringUtils;
import org.apache.flume.Event;
import org.apache.flume.EventDeliveryException;
import org.apache.hadoop.hbase.util.Bytes;

import java.io.FileInputStream;
import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import velassco.datainjection.bean.MessageBean;
import velassco.datainjection.bean.Response;
import velassco.datainjection.bean.Response.Cell;
import velassco.datainjection.bean.ResponseXML;
import velassco.datainjection.bean.ResponseXML.CellXML;
import velassco.datainjection.bean.ResponseXML.RowXML;
import velassco.datainjection.util.flume.agent.FlumeAVROManager;
import velassco.datainjection.util.HBaseManager;


@Path("data") //datablaster/rest/data/inject?path,idAgent,numPart
public class SimulationManager {
	
	/* Static HBase instance */
	private HBaseManager hbase;

	/*  Static Flume Agents */
	private FlumeAVROManager flumeGenericAgent;
	
	/* Protected data file PATHs */ 
	protected String HDFS_PATH="hdfs://khresmoi3.atosresearch.eu/velassco/DEM";
	protected String WINDOWS_PATH = "C:\\Projects\\VELaSSCo\\DataBlaster\\input\\";
	protected String UNIX_PATH = "/home/velassco/";	
	protected String NODE_CLUSTER_PATH="/localfs/home/velassco/common/simulation_files/";
	
	
	
	private static final Logger log = Logger.getLogger(SimulationManager.class.getName() );
	
//	public SimulationManager() throws Exception {	
//		this.hbase = HBaseManager.getInstance();
//		log.log(Level.FINE, "creating HBase instance" );
//	}
	
	protected void sendFlumeEvent(String portId, byte[] data) {
		// TODO Auto-generated method stub
    	flumeGenericAgent = new FlumeAVROManager(portId);
    	flumeGenericAgent.sendDataToFlume(data);
    	flumeGenericAgent.cleanUp();
		log.log(Level.FINE, "sending flume event to " + portId);
	}
	
	protected void sendFlumeEvent(String portId, List<Event> events) {
		// TODO Auto-generated method stub
    	flumeGenericAgent = new FlumeAVROManager(portId);
    	flumeGenericAgent.sendEventListToFlume(events);
    	flumeGenericAgent.cleanUp();
		log.log(Level.FINE, "sending flume events to " + portId);
	}
	
//    private static byte[] GetBytes(string str)
//    {
//	    byte[] bytes = new byte[str.Length * sizeof(char)];
//	    System.Buffer.BlockCopy(str.ToCharArray(), 0, bytes, 0, bytes.Length);
//	    return bytes;
//    }
	
	// **** OSLO MEETING: using md5( FullPath, SimulationName, UserName) = 16 bytes as SimulationID
	// return MD5 hash for ROWKEY input
    public String getMD5(String input) {
        MessageDigest msg;
        String md5_32 = "";
        byte[] md5_16 = null; 
		try {
			msg = MessageDigest.getInstance("MD5");
	        msg.update(input.getBytes(), 0, input.length());
	        md5_32 = new BigInteger(1, msg.digest()).toString(16);
	        System.out.println("md5_32: " + md5_32);
	        md5_16 = hexStringToByteArray(md5_32);
	        System.out.println("md5_16: " + md5_16);
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return md5_32;
    }	

    // generate MD5 16bytes
    public static byte[] hexStringToByteArray(String s) {
	    int len = s.length();
	    byte[] data = new byte[len / 2];
	    for (int i = 0; i < len; i += 2) {
	        data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
	                             + Character.digit(s.charAt(i+1), 16));
	    }
	    return data;
	}

    // return array rowkey
	// return MD5 hash for ROWKEY input
    public byte[] getArrayRowkey(byte[] simulId, String analysisName, double step, int partitionId, boolean data) {
    	// GIUSSEPE'S CODE
    	int length_of_String = Bytes.toBytes(analysisName).length;
    	int rowkey_length = simulId.length+4+length_of_String+8+4;
    	System.out.println("rowkey_length: " + rowkey_length);
    	ByteBuffer rowkey = ByteBuffer.allocate(rowkey_length);
    	rowkey.clear();
    	rowkey.put(simulId);
    	rowkey.put(Bytes.toBytes(length_of_String));
    	rowkey.put(Bytes.toBytes(analysisName));
    	rowkey.put(Bytes.toBytes((double)step));
    	if (data)  
    		rowkey.put(Bytes.toBytes((int)partitionId))  ;
		System.out.println("*** rowkey: " + rowkey);
		// return rowkey bytes
		return rowkey.array();
    }
    
    // return UUID random ROWKEY 
    public String getUUID(){
    	//generate random UUIDs
    	UUID uuid = UUID.randomUUID ();
    	return uuid.toString();
    }


	/**
	 * This method allows data insertion on HBase database given the simulation name and analysis type as parameters
	 * @param simulationName Type of simulation input data (DEM, FEM, etc)
	 * @param analysisName Name of the simulation data file
	 * @param partitionId Identifier of simulation part
	 * @param type Type of coordinate set (only for DEM)
	 * @return A message with status of data insertion
	 * @throws IOException
	 * @throws EventDeliveryException
	 */
	@Path("sendSimulData")
	@POST
	@Produces("application/xml")
	public MessageBean sendSimulData(@QueryParam("simulationName") String simulationName, // DEM_box, FluidizedBed, Telescope, etc.
									 @QueryParam("analysisName") String analysisName, // DEM, FEM
									 @QueryParam("partitionId") String partitionId, // 0, 1, 2, etc.
									 @QueryParam("type") String type) // pep3, p3c, p3w, msh, res
								     throws IOException, EventDeliveryException{
			
			MessageBean msg = new MessageBean();
			msg.set_messageState("Simulation data sent to HBase...");
			long startTime = System.currentTimeMillis();

			try {
				if (analysisName.equals("DEM")) {
					DEMSimulationEntities DEMsimEntities = new DEMSimulationEntities(simulationName, analysisName);
					DEMsimEntities.runETLProcess(simulationName, analysisName, partitionId, type);
					log.log(Level.FINE, "running DEM simulation" );
				} else if (analysisName.equals("FEM")) {
					FEMSimulation FEMSim = new FEMSimulation(simulationName, analysisName);
					FEMSim.runETLProcess(simulationName, analysisName, partitionId, type);
					log.log(Level.FINE, "running DEM simulation" );
				}
			} catch (Exception ex){
				msg.set_messageState("...Error while sending data to HBase");
				log.log(Level.SEVERE, "...Error while sending data to HBase" + ex.toString(), ex );
			}

			long endTime   = System.currentTimeMillis();
			long totalTime = endTime - startTime; //TimeUnit.MILLISECONDS.toSeconds(endTime - startTime);
			msg.set_timeExec(Long.toString(totalTime));
			// return message
			return msg;
			
		}

    
		protected void runETLProcess(String simulationType, String fileName, String partId, String type)
				  throws IOException, InterruptedException {
				  // to be OVERRIDED
		}
		
		
    
  /********* XML Method *******/  

	/**
	 * This method is used to query the HBase table via REST API service	
	 * @param rowkey
	 * @param database
	 * @return XML query response
	 * @throws IOException
	 * @throws JSONException 
	 * @throws org.json.JSONException 
	 */
	@Path("get")
    @GET
	@Produces("application/xml")
	@Consumes("*/*")
	public ResponseXML getDataFromHBase(@QueryParam("rowkey") String rowkey, 
										@QueryParam("database") String databse) throws IOException, JSONException, org.json.JSONException {
		String data = hbase.queryTable(rowkey,databse);
		ResponseXML response = convertJSON2XML(data);
		return response; 
	}
          
	// decode/encode from base64
	private String decode(String s) {
	    return StringUtils.newStringUtf8(Base64.decodeBase64(s));
	}
	private String encode(String s) {
	    return Base64.encodeBase64String(StringUtils.getBytesUtf8(s));
	}

	// convert json
	private ResponseXML convertJSON2XML(String json) throws org.json.JSONException, JSONException{
		//convert the json string back to object  
		Gson gson = new Gson();  
		Response responseObj = gson.fromJson(json, Response.class); //fromJson(json, Response.class); 
		System.out.println(responseObj);	
				
		ResponseXML responseXML = new ResponseXML(responseObj.rows.length);
		//decoding cells
		for (int r=0; r<responseObj.rows.length; r++){
			// decoding key
			String keyUndecoded = responseObj.rows[r].getKey();
			byte[] decodedData = Base64.decodeBase64(keyUndecoded.getBytes());
			String strDecoded = new String(decodedData);
			responseObj.rows[r].setKey(strDecoded);
			RowXML rowXML = new RowXML(responseObj.rows[r].getCells().length);
			responseXML.rows[r] = rowXML;
			rowXML.setKey(strDecoded);

			// decoding cells fields
			int index = 0;
			for (Cell c : responseObj.rows[r].getCells()){
				// decoding column
				String column = c.getColumn();
				byte[] columnDecoded = Base64.decodeBase64(column.getBytes());
				String columnStrDecoded = new String(columnDecoded);
				c.setColumn(columnStrDecoded);
						
				// decoding timestamp
				float timestamp = c.getTimestamp();
						
				// decoding $
				String $ = c.get$();
				byte[] $Decoded = Base64.decodeBase64($.getBytes());
				String $StrDecoded = new String($Decoded);
				c.set$($StrDecoded);
				
				// adding c
				CellXML xmlCell = new CellXML();
				xmlCell.setValue($StrDecoded);
				xmlCell.setColumn(columnStrDecoded);
				xmlCell.setTimestamp(timestamp);
				rowXML.setCell(index,xmlCell);
				index++;
			}				
		}
		/* Parsing to JSON String */
		//String jsonString = gson.toJson(responseObj);
		
		/* Return XML */
		return responseXML;
	}


}
