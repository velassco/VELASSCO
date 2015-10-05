package velassco.datainjection.resources;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.flume.Event;
import org.apache.flume.event.EventBuilder;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.util.Bytes;

public class DEMSimulationEntities extends DEMSimulation {
	
	private static final Logger log = Logger.getLogger(DEMSimulation.class.getName() );
	private String SIMUL_ID = "";
	private String SIMULID_METADATA;
	private String SIMULID_DATA;

	public DEMSimulationEntities() throws Exception {
		super();
	}
	
	public DEMSimulationEntities(String simulationName, String analysisName) throws Exception {
		super();
		// TODO Auto-generated constructor stub
	}

	@Override
	protected void runETLProcess(String simulationName, String analysisName, String partId, String type) 
			  throws IOException, InterruptedException {
		log.log(Level.FINE, "running ETL process" );
	
		// agent.sinks.hbaseSink.serializer.colNames=ROW_KEY,nm,fp,bb,vs,np,uName,uAcces,ot
		String nm = analysisName;
		String fp = NODE_CLUSTER_PATH + simulationName;
		String bb = "empty";
		String vs = "Not validated";
		String np = partId;
		String uName = "mtinte";
		String uAccess = "rwd";
		String uALL = "read_only";
		String ot = "NULL"; 
		
		String strKey = NODE_CLUSTER_PATH + simulationName + uName;
		SIMUL_ID = getMD5(strKey);
		String regExp = "nm:string=" + nm + "||fp:string=" + fp + "||bb:string=" + bb + "||vs:string=" + vs + "||np:string=" + np + "||uName:string=" + uName + "||uAccess:string=" + uAccess + "||uALL:string=" + uALL + "||ot:string=" + ot + "||";
		String data = SIMUL_ID + "delim" + regExp;
		sendFlumeEvent("velassco_models.propertiesCF", data.getBytes()); //rowkeyConcat);
		readFile(analysisName,simulationName,partId,type);
	}
	
//	public void testToDouble() throws Exception {
//	    double [] doubles = {Double.MIN_VALUE, Double.MAX_VALUE};
//	    for (int i = 0; i < doubles.length; i++) {
//	      byte [] b = Bytes.toBytes(doubles[i]);
//	      assertEquals(doubles[i], Bytes.toDouble(b));
//	      byte [] b2 = bytesWithOffset(b);
//	      assertEquals(doubles[i], Bytes.toDouble(b2, 1));
//	    }
//	  }
	
	private void readFile(String analysisName, String simulationName, String partitionId, String type) throws IOException, InterruptedException {
		// reading the file line by line
		BufferedReader br = null;
//		BufferedReader br_p3p = null;
//		BufferedReader br_p3c = null;
//		BufferedReader br_p3w = null;
		try {
	        // *** LOCAL file ***
			br = new BufferedReader(new FileReader(NODE_CLUSTER_PATH + simulationName + "." + type));
//	        br_p3p = new BufferedReader(new FileReader(NODE_CLUSTER_PATH + analysisName + ".p3p"));
//	        br_p3c = new BufferedReader(new FileReader(NODE_CLUSTER_PATH + analysisName + ".p3c"));
//	        br_p3w = new BufferedReader(new FileReader(NODE_CLUSTER_PATH + analysisName + ".p3w"));
	        
			// ********************
	        // readHDFSFile();
		} catch (Exception ex){
			log.log(Level.SEVERE, ex.toString(), ex );
		}
		
		
		try {
	        String line = br.readLine();
	        // file parameters
	        String timestep = "true";
	        String contacts = "false";
	        String particles = "false";
	        //append to file
	        while (line != null) {
	        	// 1st line
	        	if (line.contains("TIMESTEP")){
	        		if (line.contains("CONTACTS")) {
		        		// 2nd line
		        		line = br.readLine();
		        		StringTokenizer tokenizer = new StringTokenizer(line);
	    				timestep = tokenizer.nextToken();
	    				contacts = tokenizer.nextToken();
	    				
	    				// generating Keys
	    				SIMULID_METADATA = SIMUL_ID + "+" + analysisName + "+" + timestep; //getArrayRowkey(SIMUL_ID, analysisName, Double.parseDouble(timestep), Integer.parseInt(partitionId), false); // variable bytes
	    				System.out.println("SIMULID_METADATA: " + SIMULID_METADATA);
	    				
	    				// generating Keys
	    				SIMULID_DATA = SIMUL_ID + "+" + analysisName + "+" + timestep + "+" + partitionId; //getArrayRowkey(SIMUL_ID, analysisName, Double.parseDouble(timestep), Integer.parseInt(partitionId), true); // variable bytes
	    				System.out.println("SIMULID_DATA: " + SIMULID_DATA);
	    				
	    				// 3rd line
	    				line = br.readLine();
	    				    					

	    				// METADATA
    					insertMetadata_C(type,SIMULID_METADATA,contacts);
    					insertMetadata_M(type,SIMULID_METADATA,contacts);
    					insertMetadata_G(type,SIMULID_METADATA);
    					insertMetadata_R(type,SIMULID_METADATA);
    					// event lists
    					List<Event> eventsCM = new ArrayList<Event>();
    					List<Event> eventsR = new ArrayList<Event>();
    					int globalEvents = 0;
	    				// 4th line to ahead
	    				for (int i=0; i< Integer.parseInt(contacts); i++){ // LOOP PARA EL NUMBER OF COORDINATES
	    					line = br.readLine();
	    					// split line tokens
	    					String[] values = line.split("\\ ");
	    					String id  = Integer.toString(i);
	    					String cid_value =  "3double=" + values[2] + " " + values[3] + " " + values[4]; 
	    					if (type.equals("p3c")) {
		    					String mcn_value = "3integer=" + values[0] + " " + values[1] + " " + id;
			    				String mgp_value = "1integer=" + values[1]; 
	    						insertSimulationData_CM(type, SIMULID_DATA, id, cid_value, mcn_value, mgp_value, null, eventsCM);
	    						String r4vl_value = "3double=" + values[5] + " " + values[6] + " " + values[7];
		    					insertSimulationData_R(type, SIMULID_DATA, id, contacts, null, null, null, r4vl_value, null, eventsR);
		    					// clearing R buffers
	    					} else if (type.equals("p3w")){
	    						String mcn_value = "2integer=" + values[0] + " " + id;
	    						String mgp_value = "1integer=" + values[1];
	    						insertSimulationData_CM(type, SIMULID_DATA, id, cid_value, mcn_value, mgp_value, null, eventsCM);
		    					String r5vl_value = "3double=" + values[5] + " " + values[6] + " " + values[7];
		    					insertSimulationData_R(type, SIMULID_DATA, id, contacts, null , null, null, null, r5vl_value, eventsR);
		    					// clearing R buffers
	    					}	
	    					// checking size of events minor than batch size
	    					if (globalEvents < 998)  
	    						globalEvents++;
	    					else {
	    	    				sendFlumeEvent("globalTable.DEM.simulationData.M",eventsCM);
	    	    				sendFlumeEvent("globalTable.DEM.simulationData.R",eventsR);
	    	    				eventsCM.clear();
	    	    				eventsR.clear();
	    	    				globalEvents = 0;
	    					}
	    				}
//	    				// we send in case there is still remaining events
	    				if (globalEvents > 0){
	    					sendFlumeEvent("globalTable.DEM.simulationData.M",eventsCM);
	    					sendFlumeEvent("globalTable.DEM.simulationData.R",eventsR);
	    					System.out.println("eventsCM: "  + eventsCM.size() + " - eventsR: " + eventsR.size());
	    					eventsCM.clear();
	    					eventsR.clear();
	    				}
	        		} 
	        		else 
	        		{
		        		// 2nd line
		        		line = br.readLine();
		        		StringTokenizer tokenizer = new StringTokenizer(line);
	    				timestep = tokenizer.nextToken();
	    				particles = tokenizer.nextToken();
	    				
	    				// generating Keys
	    				SIMULID_METADATA = SIMUL_ID + "+" + analysisName + "+" + timestep; 
	    				//SIMULID_METADATA = getArrayRowkey(SIMUL_ID, analysisName, Double.parseDouble(timestep), Integer.parseInt(partitionId), false); // variable bytes
	    				System.out.println("SIMULID_METADATA: " + SIMULID_METADATA);
	    				
	    				// generating Keys
	    				SIMULID_DATA = (SIMUL_ID + "+" + analysisName + "+" + timestep + "+" + partitionId); 
	    				//SIMULID_DATA = getArrayRowkey(SIMUL_ID, analysisName, Double.parseDouble(timestep), Integer.parseInt(partitionId), true); // variable bytes
	    				System.out.println("SIMULID_DATA: " + SIMULID_DATA);
	    				
	    				// 3rd line
	    				line = br.readLine();
	    				
    					// METADATA
    					insertMetadata_C(type,SIMULID_METADATA,particles);
    					insertMetadata_M(type,SIMULID_METADATA,particles);
    					insertMetadata_G(type,SIMULID_METADATA);
    					insertMetadata_R(type,SIMULID_METADATA);

    					// event lists
    					List<Event> eventsCM = new ArrayList<Event>();
    					List<Event> eventsR = new ArrayList<Event>();
    					int globalEvents = 0;
	    				// 4th line to ahead
	    				for (int i=0; i< Integer.parseInt(particles); i++){ // LOOP PARA EL NUMBER OF COORDINATES
	    					line = br.readLine();
	    					// split line tokens
	    					String[] values = line.split("\\ ");
	    					String cid_value =  "3double=" +values[5] + " " + values[6] + " " + values[7]; 
	    					String id  = values[0];
	    					String mcn_value = "1integer=" + id;
		    				String mgp_value = "1integer=" + values[1]; 
	    					//r=((3/(4*pi))*V)^(1/3)
	    					double radius = Math.pow( ( (3.0 * Double.parseDouble(values[3])) /(4 * Math.PI) ), 1.0/3.0);
		    				String mrd_value = "1double=" + Double.toString(radius);	 
    						insertSimulationData_CM(type, SIMULID_DATA, id, cid_value, mcn_value, mgp_value, mrd_value, eventsCM);
	    					String r1vl_value = "1double=" + values[3];
    						String r2vl_value = "1double=" + values[4];
	    					String r3vl_value = "3double=" + values[8] + " " + values[9] + " " + values[10];
	    					insertSimulationData_R(type, SIMULID_DATA, id, particles, r1vl_value, r2vl_value, r3vl_value, null, null, eventsR);
	    					
	    					if (globalEvents < 998)  
	    						globalEvents++;
	    					else {
	    	    				sendFlumeEvent("globalTable.DEM.simulationData.M",eventsCM);
	    	    				sendFlumeEvent("globalTable.DEM.simulationData.R",eventsR);
	    	    				eventsCM.clear();
	    	    				eventsR.clear();
	    	    				globalEvents = 0;
	    					}
	    				}
//	    				// we send in case there is still remaining events
	    				if (globalEvents > 0){
	    					sendFlumeEvent("globalTable.DEM.simulationData.M",eventsCM);
	    					sendFlumeEvent("globalTable.DEM.simulationData.R",eventsR);
	    					System.out.println("eventsCM: "  + eventsCM.size() + " - eventsR: " + eventsR.size());
	    					eventsCM.clear();
	    					eventsR.clear();
	    				}
	        		}
	        	}	
			    // continue reading lines
		        line = br.readLine();
		        // increment counter
		        //count++;
	        }

	    } finally {
	        br.close();
	        log.log(Level.FINE, "closing simulation data file");
	    }
	}


	// CF:C stands for Coordinates with following colNames
	private void insertMetadata_C(String type, String key, String numberCoordinates) {
		// colNames defined 
		String cnm, cnc = ""; 
        String expReg = "";
        
        if(type.equals("p3c")) {
        	// C Coordinate set definitions
        	cnm = "string=p3c";
            cnc = "1integer=" + numberCoordinates;
            expReg = "c000002nm:" +cnm + "||c2nc:" + cnc;
        } else if(type.equals("p3p")) {
        	// C Coordinate set definitions
        	cnm = "string=p3p";
            cnc = "1integer="+numberCoordinates;
            expReg = "c000001nm:"+ cnm +"||c1nc:"+ cnc;
        } else if(type.equals("p3w")) {
        	// C Coordinate set definitions
            cnm = "string=p3w";
            cnc = "1integer="+numberCoordinates;
            expReg = "c000003nm:" + cnm + "||c3nc:"+ cnc;
        } else {
            log.log(Level.SEVERE, "simulation type not supported!");
        }
        
      // Send Event to Flume
	  System.out.println("globalTable.DEM.metadata.C: " + key + "delim" + expReg);
      String data = key + "delim" + expReg + "||";
      sendFlumeEvent("globalTable.DEM.metadata.C", Bytes.toBytes(data));
    }

	
	protected void insertQualifierValuesMetadataM(String qualifier_id, String un, String mnm, String mcn, String met, String mne, String mcl, int mnn, byte[] key) {
//
//		// Buffers for Metadata_M
//		ByteBuffer un_label = ByteBuffer.allocate(16);
//		ByteBuffer mnm_label = ByteBuffer.allocate(16);
//		ByteBuffer mcn_label = ByteBuffer.allocate(16);
//		ByteBuffer met_label = ByteBuffer.allocate(16);
//		ByteBuffer mne_label = ByteBuffer.allocate(16);
//		ByteBuffer mnn_label = ByteBuffer.allocate(16);
//		ByteBuffer mcl_label = ByteBuffer.allocate(16);
//		ByteBuffer un_values = ByteBuffer.allocate(24);		
//		ByteBuffer mnm_values = ByteBuffer.allocate(24);	// TO BE LIMITED
//		ByteBuffer mcn_values = ByteBuffer.allocate(24);	// TO BE LIMITED	
//		ByteBuffer met_values = ByteBuffer.allocate(24);	// TO BE LIMITED
//		ByteBuffer mne_values = ByteBuffer.allocate(8);		// 64-bit (8 bytes) integer, number of elements of Mesh 1
//		ByteBuffer mnn_values = ByteBuffer.allocate(4);		// 4 bytes for integer??
//		ByteBuffer mcl_values = ByteBuffer.allocate(24);	// TO BE LIMITED
//
//		un_label.put("un:".getBytes());
//		un_values = ByteBuffer.allocate(un.length());
//		un_values.put(un.getBytes());
//		mnm_label.put(("||m" + qualifier_id + "nm:").getBytes());
//		mnm_values = ByteBuffer.allocate(mnm.length());
//		mnm_values.put(mnm.getBytes());
//		mcn_label.put(("||m" + qualifier_id + "cn:").getBytes());
//		mcn_values = ByteBuffer.allocate(mcn.length());
//		mcn_values.put(mcn.getBytes());
//		met_label.put(("||m" + qualifier_id +"et:").getBytes());
//		met_values = ByteBuffer.allocate(met.length());
//		met_values.put(met.getBytes());
//		mne_label.put(("||m" + qualifier_id +"ne:").getBytes());
//		mne_values.put(Bytes.toBytes(Integer.parseInt(mne))); // INTEGER 4 Bytes
//		mnn_label.put(("||m" + qualifier_id +"nn:").getBytes());
//		mnn_values.put(Bytes.toBytes(mnn)); // INTEGER 4 Bytes
//		mcl_label.put((("||m" + qualifier_id +"cl:").getBytes()));
//		mcl_values = ByteBuffer.allocate(mcl.length());
//		mcl_values.put(mcl.getBytes()); 
//
//		// Send Event to Flume
//		ByteArrayOutputStream outputStream = new ByteArrayOutputStream( );
//		try {
//			outputStream.write( key );
//			outputStream.write("delim".getBytes());
//			outputStream.write(un_label.array());
//			outputStream.write(un_values.array());
//			outputStream.write(mnm_label.array());
//			outputStream.write(mnm_values.array());
//			outputStream.write(mcn_label.array());
//			outputStream.write(mcn_values.array());
//			outputStream.write(met_label.array());
//			outputStream.write(met_values.array());
//			outputStream.write(mne_label.array());
//			outputStream.write(mne_values.array());
//			outputStream.write(mnn_label.array());
//			outputStream.write(mnn_values.array());
//			outputStream.write(mcl_label.array());
//			outputStream.write(mcl_values.array());
//			outputStream.write("||".getBytes());
//		} catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//		byte[] rowkeyConcat= outputStream.toByteArray( );
//		sendFlumeEvent("globalTable.DEM.metadata.M", rowkeyConcat);
//		// cleating buffers
//		outputStream.reset();
//		rowkeyConcat = null;
 	}

	// CF:M stands for Mesh with following colNames
    private void insertMetadata_M(String type, String key, String numberCoordinates) {
    	// colNames defined 
    	String un, mnm, mcn, met, mne, mcl;	
    	String expReg = "";
    	// met values: { lines, triangles, tetrahedrons and spheres }
    	   
    	if(type.equals("p3p")) {
    		// units
    		un = "string=empty";
    		// M mesh definitions
    		mnm = "string=particles";
    		mcn = "string=c1";
    		met = "string=sphere";
    		mne = "1integer=" + numberCoordinates;
    		String mnn = "1integer=" + 1;
    		mcl = "string=no_colour";
    		//String qualifier_id = "000001";
    		//insertQualifierValuesMetadataM(qualifier_id, un, mnm, mcn, met, mne, mcl, mnn, key);		
    		expReg = "un:" +un +"||m000001nm:"+ mnm + "||m000001cn:"+ mcn + "||m000001et:" + met + "||m000001ne:" + mne + "||m000001nn:" + mnn + "||m000001cl:" + mcl;
    	} else if(type.equals("p3c")) {
    		// units
    		un = "string=m";    	
    		// M mesh definitions
    		mnm = "string=p2p contacts";
    		mcn = "string=c2";
    		met = "string=quadratic line";
    		mne = "1integer=" + numberCoordinates;
    		String mnn = "1integer=" + 3;
    		mcl = "string=no_colour";
    		//String qualifier_id = "string=000002";
    		//insertQualifierValuesMetadataM(qualifier_id, un, mnm, mcn, met, mne, mcl, mnn, key);
    		expReg = "un:" +un +"||m000002nm:"+ mnm + "||m000002cn:"+ mcn + "||m000002et:" + met + "||m000002ne:" + mne + "||m000002nn:" + mnn + "||m000002cl:" + mcl;
    		
    	} else if(type.equals("p3w")) {
    		// units
    		un = "string=m";
    		// M mesh definitions
    		mnm = "string=p2w contacts";
    		mcn = "string=c3";
    		met = "string=line";
    		mne = "1integer=" + numberCoordinates;
    		String mnn = "1integer=" + 2;
    		mcl = "string=no_colour";
    		//String qualifier_id = "000003";
    		//insertQualifierValuesMetadataM(qualifier_id, un, mnm, mcn, met, mne, mcl, mnn, key);
    		expReg = "un:" +un +"||m000003nm:"+ mnm + "||m000003cn:"+ mcn + "||m000003et:" + met + "||m000003ne:" + mne + "||m000003nn:" + mnn + "||m000003cl:" + mcl;

    	} else {
    		log.log(Level.SEVERE, "simulation type not supported!");
    	}
    	        
    	System.out.println("globalTable.DEM.metadata.M: " + key + "delim" + expReg);
    	String data = key + "delim" + expReg + "||";
    	sendFlumeEvent("globalTable.DEM.metadata.M", Bytes.toBytes(data));       
    }

    

	// CF:G stands for Gauss Points with following colNames
    private void insertMetadata_G(String type, String key) {
    	// colNames defined 
    	String gnm, get, gni, gic, gnc, gmn;	
    	String expReg = "";

    	if (type.equals("p3c")) {
    		// DO NOTHING YET

    	} else if (type.equals("p3p")) {
    		expReg = "";
    		gnm = "string=gp_p2p";
    		get = "string=line";
    		String gng = "1integer=" + 1;
    		gni = "string=false";
    		gic = "string=false";
    		gnc = "string=empty";
    		gmn = "string=p2p_contacts";
    		expReg = "g000001nm:"+ gnm + "||g000001et:"+ get + "||g000001ng:" + gng + "||g000001ni:" + gni + "||g000001ic:" + gic + "||g000001nc:" + gnc + "||g000001mn:" + gmn;
        	System.out.println("globalTable.DEM.metadata.G: " + key + "delim" + expReg);
        	String data = key + "delim" + expReg + "||";
        	sendFlumeEvent("globalTable.DEM.metadata.G", Bytes.toBytes(data)); 
    	} else if (type.equals("p3w")) {
    		expReg = "";
    		gnm = "string=gp_p2w";
    		get = "string=line";
    		String gng = "1integer=" + 1;
    		gni = "string=false";
    		gic = "string=false";
    		gnc = "string=empty";
    		gmn = "string=p2w_contacts";
    		expReg = "g000002nm:"+ gnm + "||g000002et:"+ get + "||g000002ng:" + gng + "||g000002ni:" + gni + "||g000002ic:" + gic + "||g000002nc:" + gnc + "||g000002mn:" + gmn;
        	System.out.println("globalTable.DEM.metadata.G: " + key + "delim" + expReg);
        	String data = key + "delim" + expReg + "||";
        	sendFlumeEvent("globalTable.DEM.metadata.G", Bytes.toBytes(data)); 
    	} else {
    		log.log(Level.SEVERE, "simulation type not supported!");
    	}	

    }

    
    
    //CF:R stands for Results with following colNames
    private void insertMetadata_R(String type, String key) {
        
    	// colNames defined 
    	String rnm, rrt, rcn, rlc, rgp, run;
    	String expReg = "";
        String data = "";
        
        
    	if(type.equals("p3c")) {
    		
    		// R result definitions Position
            expReg = "";
            rnm = "string=Force p2p";
            rrt = "string=vector";
            String rnc4 = "1integer=" + 3;
            rcn = "string={fx, fy, fz}";
            rlc = "string=OnGaussPoints";
            rgp = "string=gp_p2p";
            run = "string=N";
    		// labels
            expReg = "r000004nm:" + rnm + "||r000004rt:" + rrt + "||r000004nc:" + rnc4 + "||r000004cn:" + rcn + "||r000004lc:"+ rlc + "||r000004gp:" + rgp + "||r000004un:" + run;
    		
    		// Send Event to Flume
            System.out.println("globalTable.DEM.metadata.R: " + key + "delim" + expReg);
            data = key + "delim" + expReg + "||";
            sendFlumeEvent("globalTable.DEM.metadata.R", Bytes.toBytes(data));
    		
        } else if(type.equals("p3p")) {

        	// R result definitions Position
            expReg = "";
        	rnm = "string=Volume";
            rrt = "string=scalar";
            String rnc1 = "1integer=" + 1;
            rcn = "string=empty";
            rlc = "string=OnNodes";
            rgp = "string=empty";
            run = "string=empty";            
            expReg = "r000001nm:" + rnm + "||r000001rt:" + rrt + "||r000001nc:" + rnc1 + "||r000001cn:" + rcn + "||r000001lc:"+ rlc + "||r000001gp:" + rgp + "||r000001un:" + run;
    		
//    		// Send Event to Flume
            System.out.println("globalTable.DEM.metadata.R: " + key + "delim" + expReg);
            data = key + "delim" + expReg + "||";
            sendFlumeEvent("globalTable.DEM.metadata.R", Bytes.toBytes(data)); 
        	
            expReg = "";
            rnm = "string=Mass";
            rrt = "string=scalar";
            String rnc2 = "1integer=" + 1;
            rcn = "string=empty";
            rlc = "string=OnNodes";
            rgp = "string=empty";
            run = "string=empty";
            
            expReg = "r000002nm:" + rnm + "||r000002rt:" + rrt + "||r000002nc:" + rnc2 + "||r000002cn:" + rcn + "||r000002lc:"+ rlc + "||r000002gp:" + rgp + "||r000002un:" + run;
            
    		// Send Event to Flume
            System.out.println("globalTable.DEM.metadata.R: " + key + "delim" + expReg);
            data = key + "delim" + expReg + "||";
            sendFlumeEvent("globalTable.DEM.metadata.R", Bytes.toBytes(data));
            
        	
            expReg = "";
        	rnm = "string=Velocity";
            rrt = "string=vector";
            String rnc3 = "1integer=" + 3;
            rcn = "string={vx, vy, vz}";
            rlc = "string=OnNodes";
            rgp = "string=empty";
            run = "string=empty"; 
    		   
            expReg = "r000003nm:" + rnm + "||r000003rt:" + rrt + "||r000003nc:" + rnc3 + "||r000003cn:" + rcn + "||r000003lc:"+ rlc + "||r000003gp:" + rgp + "||r000003un:" + run;
    		
            // Send Event to Flume
            System.out.println("globalTable.DEM.metadata.R: " + key + "delim" + expReg);
            data = key + "delim" + expReg + "||";
            sendFlumeEvent("globalTable.DEM.metadata.R", Bytes.toBytes(data)); 
        	
        } else if(type.equals("p3w")) {
    		// R result definitions Position
            expReg = "";
            rnm = "string=Force p2w";
            rrt = "string=vector";
            String rnc5 = "1integer=" + 3;
            rcn = "string={fx, fy, fz}";
            rlc = "string=OnGaussPoints";
            rgp = "string=gp_p2w";
            run = "string=N";
            expReg = "r000005nm:" + rnm + "||r000005rt:" + rrt + "||r000005nc:" + rnc5 + "||r000005cn:" + rcn + "||r000005lc:"+ rlc + "||r000005gp:" + rgp + "||r000005un:" + run;
            
            // Send Event to Flume
            System.out.println("globalTable.DEM.metadata.R: " + key + "delim" + expReg);
            data = key + "delim" + expReg + "||";
            sendFlumeEvent("globalTable.DEM.metadata.R", Bytes.toBytes(data));
    		
        } else {
        	log.log(Level.SEVERE, "simulation type not supported!");
        }	 
    }
       
	
    								//**************************************************************************************//
    
    // CF:C stands for Coordinates with following colNames
	// CF:M stands for Mesh with following colNames
	private void insertSimulationData_CM(String type, String keyData, String id, String cid_value, String mcn_value, String mgp_value, String mrd_value,
										 List<Event> events) {

		String expReg = "";
		if (type.equals("p3c")) {
			expReg = "c000002_" + id + ":"  + cid_value + "||m000002cn_" + id + ":" + mcn_value + "||m000002gp_" + id + ":" + mgp_value;  //+ "||m2rd_ " + id + ":" + mrd_value; 
		} else if (type.equals("p3p")) {
			expReg = "c000001_" + id + ":"  + cid_value + "||m000001cn_" + id + ":" + mcn_value + "||m000001gp_" + id + ":" + mgp_value + "||m000001rd_" + id + ":" + mrd_value; 
		} else if (type.equals("p3w")) {
			expReg = "c000003_" + id + ":"  + cid_value + "||m000003cn_" + id + ":" + mcn_value + "||m000003gp_" + id + ":" + mgp_value; //+ "||m3rd_ " + id + ":" + mrd_value; 
		} else {
			log.log(Level.SEVERE, "simulation type not supported!");
		}
		System.out.println("globalTable.DEM.simulationData.CM" + keyData + "delim"  +  expReg);
		String data = keyData + "delim" + expReg + "||";
//		sendFlumeEvent("globalTable.DEM.simulationData.M", data.getBytes());
		Event event = EventBuilder.withBody(data.getBytes()); 
	    events.add(event);
	}
	
	// CF:R stands for Results with following colNames
	private void insertSimulationData_R(String type, String keyData, String id, String number_pc, String r1vl_value, String r2vl_value, String r3vl_value, 
									    String r4vl_value, String r5vl_value, List<Event> events) {
		
		String expReg = "";
		if (type.equals("p3p")) {
			expReg = "r000001vl_" + id + ":" + r1vl_value + "||r000002vl_" + id + ":" + r2vl_value + "||r000003vl_" + id + ":" + r3vl_value;
			String data = keyData + "delim" + expReg + "||";
			System.out.println("globalTable.DEM.simulationData.R" + data);
//			sendFlumeEvent("globalTable.DEM.simulationData.R", data.getBytes());
			Event event = EventBuilder.withBody(data.getBytes()); 
		    events.add(event);
		    
		} else if (type.equals("p3c")) {

			expReg = "r000004vl_" + id + ":" + r4vl_value;
			String data = keyData + "delim" + expReg + "||";
			System.out.println("globalTable.DEM.simulationData.R" + data);
//			sendFlumeEvent("globalTable.DEM.DEM.simulationData.R", data.getBytes());
			Event event = EventBuilder.withBody(data.getBytes()); 
		    events.add(event);
		    
		} else if (type.equals("p3w")) {			

			expReg = "r000005vl_" + id + ":" + r5vl_value; 

			String data = keyData + "delim" + expReg + "||";
			System.out.println("globalTable.DEM.simulationData.R" + data);
//			sendFlumeEvent("globalTable.DEM.simulationData.R", data.getBytes());
			Event event = EventBuilder.withBody(data.getBytes()); 
		    events.add(event);
		    
		} else {
			log.log(Level.SEVERE, "simulation type not supported!");
		}
	   
	}
	

}
