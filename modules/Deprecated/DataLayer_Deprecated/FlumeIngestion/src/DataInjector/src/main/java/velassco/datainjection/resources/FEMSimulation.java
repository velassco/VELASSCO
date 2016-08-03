package velassco.datainjection.resources;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.commons.lang.StringUtils;
import org.apache.flume.Event;
import org.apache.flume.event.EventBuilder;
import org.apache.hadoop.hbase.util.Bytes;

public class FEMSimulation extends SimulationManager {

	private static final Logger log = Logger.getLogger(FEMSimulation.class.getName() );
	private String SIMUL_ID = null;
//	private String SIMULID_METADATA;
//	private String SIMULID_DATA;
	//protected String FEM_PATH = NODE_CLUSTER_PATH + "VELaSSCo_HbaseBasicTest_FEM/"; ///localfs/home/velassco/common/simulation_files/VELaSSCo_HbaseBasicTest_FEM
	
	public FEMSimulation() throws Exception {
		super();
	}
	
	public FEMSimulation(String simulationName, String analysisName) throws Exception {
		super();
	}	
		// TODO Auto-generated constructor stub
		
	@Override
	protected void runETLProcess(String simulationName, String analysisName, String partId, String type) throws IOException, InterruptedException {
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
				readFile(analysisName,fp,partId,type);
	}


	
	private void readFile(String analysisName, String fp, String partitionId, String type) throws IOException, InterruptedException {
		// reading the file line by line
		
		
		// **** LOOP FOR SEVERAL SIMULATION FILES ***//
//		File folder = new File(fp);
//		for (final File fileEntry : folder.listFiles()) {
//			System.out.println(fileEntry.getName());
			try {
				// *** LOCAL file ***
				if (type.equals("msh")) {
					BufferedReader br_msh = null;
					br_msh = new BufferedReader(new FileReader(fp + ".msh"));
					try {
						readMeshFile(br_msh,partitionId);
					} finally {
						br_msh.close();
						log.log(Level.FINE, "closing MESH simulation data file");
					}

				} else if (type.equals("res")) {
					BufferedReader br_res = null;
					br_res = new BufferedReader(new FileReader(fp + ".res"));
					try {
						readResultFile(br_res,partitionId);
					} finally {
						br_res.close();
						log.log(Level.FINE, "closing RES simulation data file");
					}
				}	
			} catch (Exception ex){
				log.log(Level.SEVERE, ex.toString(), ex );
			}
//		}	
	}




	/* 
	 * FILE FORMAT .MSH:
	 * # encoding utf-8
	 * 	MESH "Layer0" dimension 3 ElemType Triangle Nnode 3
	 * 	# Color 0.505882 0.505882 0.505882
	 * 	Coordinates
	 * 	...
	 * 	end coordinates
	 * 	Elements
	 * 	..
	 * 	end elements
	 * 
	 * 	MESH "ground" dimension 3 ElemType Triangle Nnode 3
	 * 	Coordinates
	 * 	end coordinates
	 *  Elements
	 *  ...
	 *  end elements
	 *  
	 *  MESH "cone" dimension 3 ElemType Triangle Nnode 3
	 *  Coordinates
	 *  end coordinates
	 *  Elements
	 *  end elements
	 *  
	 *  MESH "prism" dimension 3 ElemType Triangle Nnode 3
	 *  # Color 0.000000 1.000000 0.000000
	 *  Coordinates
	 *  end coordinates
	 *  Elements
	 *  ...
	 *  end Elements
	 *  
	 *  MESH "roof" dimension 3 ElemType Triangle Nnode 3
	 *	Coordinates
	 *	end coordinates
	 *  Elements
	 *  ...
	 *  end Elements
	 */
	private void readMeshFile(BufferedReader br_msh, String partitionId) throws IOException {
		

	    // *** STATIC MESH NAME="" AND TIMESTEP=0.0
		// generating Keys
		String SIMULID_METADATA = SIMUL_ID + "+" + "" + "+" + "0.0"; //getArrayRowkey(SIMUL_ID, analysisName, Double.parseDouble(timestep), Integer.parseInt(partitionId), false); // variable bytes
		System.out.println("SIMULID_METADATA: " + SIMULID_METADATA);
		
		// generating Keys
		String SIMULID_DATA = SIMUL_ID + "+" + "" + "+" + "0.0" + "+" + partitionId; //getArrayRowkey(SIMUL_ID, analysisName, Double.parseDouble(timestep), Integer.parseInt(partitionId), true); // variable bytes
		System.out.println("SIMULID_DATA: " + SIMULID_DATA);
		
		// global parameters
		String[] tokens;
    	int count = 0;
		// first line
        String line_msh = br_msh.readLine();
    	// qualifiers
		String cnm = "empty";
		String cnc = "empty";
		String met = "empty";
		String mnn = "empty";
		String mne = "empty";
		String mnm = "empty";
		String dimension = "empty";
	    String mid = "";
	    String mgr = "empty";
	    long block_count = 000000;
	    
		
        while (line_msh != null) {
			
        	// 1st line
        	if (line_msh.contains("MESH")){
        		tokens = line_msh.split("\\ ");
        		mnm = tokens[1];
        		dimension = tokens[3];
    			met = tokens[5];
    			mnn = tokens[7];
    			cnm="Part coordinates";  
    			block_count++;
    			// Rowkey = ( SimID + "" + 0.0) 
    	    	// rowkey = SIMUL_ID + block_count;
	        } else if (line_msh.equalsIgnoreCase("Coordinates")){
	        	count = 0;
    			// continue reading lines
    			line_msh = br_msh.readLine();
	        	while (!line_msh.equalsIgnoreCase("end coordinates")){
	        			tokens = line_msh.split("\\ ");
	        			String node_id = tokens[0];
	        			String coordinates_x = tokens[1];
	        		    String coordinates_y = tokens[2];
	        		    String coordinates_z =  (tokens.length==4)? tokens[3]: "0.0"; // SON TODAS 3D!!! ASUMIMOS Z=0 EN CASO CONTRARIO
	        		    String expReg = "c" + StringUtils.leftPad(""+block_count ,6, "0")  + "_" + node_id + ":3double=" + coordinates_x + " " + coordinates_y + " " + coordinates_z;  
	        		    String data = SIMULID_DATA + "delim"  +  expReg + "||";
	        		    sendFlumeEvent("globalTable.FEM.simulationData.M", Bytes.toBytes(data));
	        			count++;
	        			// continue reading lines
	        			line_msh = br_msh.readLine();
	        	}
	        	//c1id = "{" + c1id.substring(0, c1id.length()-1) + "}";
	        	//c1xyz = "{" + c1xyz.substring(0, c1xyz.length()-1) + "}";
	        	// one put per partition
	        	cnc = Integer.toString(count);
	        } else if (line_msh.equalsIgnoreCase("Elements")){
	        	count = 0;
    			// continue reading lines
    			line_msh = br_msh.readLine();
	        	while (!line_msh.equalsIgnoreCase("end elements")){
	        		// if tetrahedra, triangle, line, sphere - 3 dimensions
		        	tokens = line_msh.split("\\ ");
	        		mid = tokens[0];
	        		String coordinates_x = tokens[1];
	        		String coordinates_y = tokens[2];
	        		String coordinates_z = tokens[3]; // SON TODAS 3D!!! ASUMIMOS Z=0 EN CASO CONTRARIO
	        		String expReg = "";
	        		if (mnn.equals("4")) {
	        			String coordinates_w = tokens[4];
        				expReg = "m" + StringUtils.leftPad(""+block_count ,6, "0") + "_" + mid + ":4integer=" + coordinates_x + " " + coordinates_y + " " + coordinates_z + " " + coordinates_w; 
	        			if (tokens.length == 6) {
	        				mgr = tokens[5];        			
	        				expReg += "||m" + StringUtils.leftPad(""+block_count ,6, "0") + "gr_" + mid + ":1integer=" + mgr; 
	        			}	
	        		} else if (mnn.equals("3")) {
	        			expReg = "m" + StringUtils.leftPad(""+block_count ,6, "0") + "_" + mid + ":3integer=" + coordinates_x + " " + coordinates_y + " " + coordinates_z;
	        			if (tokens.length == 5) {
	        				mgr = tokens[4];
	        				expReg += "||m" + StringUtils.leftPad(""+block_count ,6, "0") + "gr_" + mid + ":1integer=" + mgr; 
	        			}	
	        		}
	        		String data = SIMULID_DATA + "delim"  +  expReg + "||";
	        		sendFlumeEvent("globalTable.FEM.simulationData.M", Bytes.toBytes(data));
	        		count++;	
	        		// continue reading lines
	        		line_msh = br_msh.readLine();
	        	} //********** m1cn_id = n1 n2 n3 n4
	        	//m1id = "{" + m1id.substring(0, m1id.length()-1) + "}";
	        	//m1cn = "{" + m1cn.substring(0, m1cn.length()-1) + "}";
	        	mne = Integer.toString(count);
    		    // reset rowkey
//        		rowkey =  SIMUL_ID + "0.0" + simulPartId;
//    		    String expReg = "c1id:" + c1id + "||c1nc:" + c1nc + "||c1xyz:" + c1xyz + "||m1id:" + m1id + "||m1cn:" + m1cn + "||m1gr:" + m1gr;  
//    		    String data = rowkey + "delim" + expReg;
//    		    //System.out.println(data);
//    		    sendFlumeEvent("globalTable.FEM.metadata.M", Bytes.toBytes(data));
    		    // reset qualifiers
    		    mid = "";
    		    mgr = "empty";
	        } 	  	
        	// continue reading lines
    		line_msh = br_msh.readLine();
        } 
		String expReg = "c" + StringUtils.leftPad(""+block_count ,6, "0") + "nm:string=" + cnm + "||c" + StringUtils.leftPad(""+block_count ,6, "0") + "nc:1integer=" + cnc + 
							"||m" + StringUtils.leftPad(""+block_count ,6, "0") + "et:string=" + met + "||m" + StringUtils.leftPad(""+block_count ,6, "0") + "nn:1integer="+ mnn +
							"||m"+ StringUtils.leftPad(""+block_count ,6, "0") + "nm:string="+ mnm + "||m" + StringUtils.leftPad(""+block_count ,6, "0") + "ne:1integer=" + mne + "||dimension:string="+ dimension;
		String data = SIMULID_METADATA + "delim" + expReg + "||";
		sendFlumeEvent("globalTable.FEM.metadata.M", Bytes.toBytes(data)); 
	}

	
	/* FORMAT FILE .RES:
	 * 
	 * GiD Post Results File 1.0
	 * 
	 * # encoding utf-8
	 * Result "Height" "geometry" 1 Vector OnNodes
	 * ComponentNames "X-Height", "Y-Height", "Z-Height", "|Height|"
	 * Values
	 * ..
	 * End values
	 * 
	 * Result "Scalar result" "geometry" 9 Scalar OnNodes
	 * ComponentNames "Scalar result"
	 * Values
	 * ..
	 * End values
	 * 
	 * Result "Height" "geometry" 10 Vector OnNodes
	 * ComponentNames "X-Height", "Y-Height", "Z-Height", "|Height|"
	 * Values
	 * ..
	 * End values
	 * 
	 * ComponentNames "Scalar result"
	 * Values
	 * ..
	 * End values
	 */
	private void readResultFile(BufferedReader br_res, String simulPartId) throws IOException {
		// first line
        String line_res = br_res.readLine();
        //System.out.println("#### FIRST LINE .RES: " + line_res);
        
//	    long block_count = 000000;
        //append to file
        while (line_res != null) {
//        	block_count++;
        	if (line_res.contains("Result") & !(line_res.contains("GiD"))) {
        		String[] tokens = line_res.split("\\ ");
        		//System.out.println(line_res);
        		//System.out.println(tokens);
//        		String resultName = tokens[1];
        		String analysisName = tokens[2];
    			String StepValue = tokens[3];
    			String resultType = tokens[4];
    			String location = tokens[5];
    			// generating Keys
    			String SIMULID_METADATA = SIMUL_ID + "+" + "FEM" + "+" + StepValue; //getArrayRowkey(SIMUL_ID, analysisName, Double.parseDouble(timestep), Integer.parseInt(partitionId), false); // variable bytes
    			System.out.println("SIMULID_METADATA: " + SIMULID_METADATA);
    			String SIMULID_DATA = SIMUL_ID + "+" + "FEM" + "+" + StepValue + "+" + simulPartId; //getArrayRowkey(SIMUL_ID, analysisName, Double.parseDouble(timestep), Integer.parseInt(partitionId), true); // variable bytes
    			System.out.println("SIMULID_DATA: " + SIMULID_DATA);
    			
    			if (location.equals("OnGaussPoints")) {
    				String location_Name = "GP_NAME";
    			}
    			//String GP_Location = tokens[6];
    			//System.out.println("Name: " + name);
    			if (resultType.contains("Scalar")){ 
    				//Simulations_Metadata: RowKey = ( SimId + "geometry" + 1 )   CF:R = ( r1nm="PartitionId", r1rt="Scalar", r1nc=1, r1lc="OnNodes", 
                    //Result "Height" "geometry" 1 Vector OnNodes
    		        //System.out.println("#### INSIDE PartitionId");
    				String r1nm = "PartitionId";
    				String r1rt = resultType;
    			    int r1nc = 1;
    			    String r1lc = location;
    			    String expReg = "r000001nm:string=" + r1nm + "||r000001rt:string=" + r1rt + "||r000001nc:1integer=" + r1nc + "||r000001lc:string=" + r1lc; //r1_id = 
        			String data = SIMULID_DATA + "delim" + expReg + "||"; 
        			sendFlumeEvent("globalTable.FEM.metadata.R", Bytes.toBytes(data));
        			// continue reading lines
                	line_res = br_res.readLine();
                	//System.out.println("BEFORE INSERT VALUES: " + line_res);
                	
        			if (line_res.equalsIgnoreCase("Values")) {
        				insertValues(SIMULID_DATA,r1nm, br_res, StepValue, simulPartId);	
                	}
        			
    			} else if (resultType.contains("Height")){
    				//Simulations_Metadata: RowKey = ( SimId + "geometry" + 1 )   CF:R = ( r2nm="Height", r2rt="Vector", r2nc=4, r2lc="OnNodes", r2nm="X-Height","Y-Height","Z-Height","|Height|")
                    //Result "Vector function" "geometry" 1 Vector OnNodes
    		        //System.out.println("#### INSIDE Height");
    				String r2nm = "Height";
    				String r2rt = resultType;
    			    int r2nc = 4;
    			    String r2lc = location;
    				// reading Component Names line
    	        	line_res = br_res.readLine();
    	        	String[] tokensCN = line_res.split("ComponentNames");
    	        	String r2cnm = tokensCN[1];
    			    String expReg = "r000002nm:string=" + r2nm + "||r000002rt:string=" + r2rt + "||r000002nc:1integer=" + r2nc + "||r000002lc:string=" + r2lc + "||r000002cnm:string=" + r2cnm;
        			String data = SIMULID_DATA + "delim" + expReg + "||";  
       				sendFlumeEvent("globalTable.FEM.metadata.R", Bytes.toBytes(data));
    				// continue reading lines
    	        	line_res = br_res.readLine();
    	        	//System.out.println("BEFORE INSERT VALUES: " + line_res);
    	        	
    				if (line_res.equalsIgnoreCase("Values")) {
    					insertValues(SIMULID_DATA,r2nm, br_res, StepValue, simulPartId);
    	        	}
    				
    			} else if (resultType.contains("Vector")){
    				//Simulations_Metadata: RowKey = ( SimId + "geometry" + 1 )   CF:R = ( r3nm="Vector function", r3rt="Vector", r3nc=4, r3lc="OnNodes", r3nm="Vx = ...","Vy = ...","Vz = ...","|Vector function|")
    		        //System.out.println("#### INSIDE Vector function");
    				String r3nm = "Vector";
      				String r3rt = resultType;
    			    int r3nc = 4;
    			    String r3lc = location;
    				// reading Component Names line
    	        	line_res = br_res.readLine();
    	        	String[] tokensCN = line_res.split("ComponentNames");
    	        	String r3cnm = tokensCN[1];
    			    String expReg = "r000003nm:string=" + r3nm + "||r000003rt:string=" + r3rt + "||r000003nc:1integer=" + r3nc + "||r000003lc:string=" + r3lc + "||r000003cnm:string=" + r3cnm;;
        			String data = SIMULID_DATA + "delim" + expReg;
       				sendFlumeEvent("globalTable.FEM.metadata.R", Bytes.toBytes(data));

    				// continue reading lines
    	        	line_res = br_res.readLine();
    	        	//System.out.println("BEFORE INSERT VALUES: " + line_res);
    	        	
    				if (line_res.equalsIgnoreCase("Values")) {
    					insertValues(SIMULID_DATA,r3nm, br_res, StepValue, simulPartId);
    	        	}
    			} 
        	} 
        	// continue reading lines
        	line_res = br_res.readLine();
        }
	}
	
	
	private void insertValues(String SIMULID_DATA, String header, BufferedReader br_res, String StepValue, String simulPartId) throws IOException {
		
		int countnr=0;
		// events var
    	List<Event> events = new ArrayList<Event>();
		int globalEvents = 0;
		
		 // while
		if (header.contains("PartitionId")){
			int r1nr = 0;
			String r1id = "";
			String r1vl = "";
			String line_res = br_res.readLine();
			//System.out.println("INSIDE INSERT VALUES: " + line_res);
			while (!line_res.equalsIgnoreCase("end Values")){
				StringTokenizer tokenizer = new StringTokenizer(line_res);
				System.out.println("**** line_res: " + line_res);
				String id = tokenizer.nextToken();
				String value = tokenizer.nextToken();
				r1id = id;
				r1vl = value;
				String expReg = "r000001vl_" + r1id + ":1double=" + r1vl;
				String data = SIMULID_DATA + "delim" + expReg + "||";
				Event event = EventBuilder.withBody(Bytes.toBytes(data)); 
			    events.add(event);
				// continue reading lines
				line_res = br_res.readLine();
				countnr++;
				// checking size of events minor than batch size
				if (globalEvents < 998)  
					globalEvents++;
				else {
        			sendFlumeEvent("globalTable.FEM.simulationData.R",events);
        			events.clear();
    				globalEvents = 0;
				}
			}
			// we send in case there is still remaining events
			if (globalEvents > 0){
				sendFlumeEvent("globalTable.FEM.simulationData.R",events);
				events.clear();
			}
			r1nr = countnr;
			
//		    String expReg = "r1_" + countnr + ":" + r1id + "||r1vl_" + countnr + ":" + r1vl;  //+ "||r1nr:" + r1nr;
			//String data = rowkeyValues + "delim" + expReg; 
//			sendFlumeEvent("globalTable.FEM.simulationData.R", events);
		} else if (header.contains("Height")){
			int r2nr = 0;  
			String r2id = "";
			String r2vl = "";
			String line_res = br_res.readLine();
			//System.out.println("INSIDE INSERT VALUES: " + line_res);
			while (!line_res.equalsIgnoreCase("end values")){
				StringTokenizer tokenizer = new StringTokenizer(line_res);
				String id = tokenizer.nextToken();;
				String x_value = tokenizer.nextToken();
				String y_value = tokenizer.nextToken();
				String z_value = tokenizer.nextToken();
				String v_module = tokenizer.nextToken();
				r2id = id;
				r2vl = x_value + " " + y_value + " " + z_value;  //+ " " + v_module;
				String expReg = "r000002vl_" + r2id + ":3double=" + r2vl;
				String data = SIMULID_DATA + "delim" + expReg + "||";
				Event event = EventBuilder.withBody(Bytes.toBytes(data));
			    events.add(event);
				// continue reading lines
				line_res = br_res.readLine();
				countnr++;
				// checking size of events minor than batch size
				if (globalEvents < 998)  
					globalEvents++;
				else {
        			sendFlumeEvent("globalTable.FEM.simulationData.R",events);
        			events.clear();
    				globalEvents = 0;
				}
			}
			// we send in case there is still remaining events
			if (globalEvents > 0){
				sendFlumeEvent("globalTable.FEM.simulationData.R",events);
				events.clear();
			}
			r2nr = countnr;
//		    String expReg = "r2id:" + r2id + "||r2vl:" + r2vl + "||r2nr:" + r2nr;
//			String data = rowkeyValues + "delim" + expReg; 
//			sendFlumeEvent("globalTable.FEM.simulationData.R", Bytes.toBytes(data));
//			sendFlumeEvent("globalTable.FEM.simulationData.R", events);
		} else if (header.contains("Vector")){
			int r3nr = 0;
			String r3id = "";
			String r3vl = "";
			String line_res = br_res.readLine();
			//System.out.println("INSIDE INSERT VALUES: " + line_res);
			while (!line_res.equalsIgnoreCase("end values")){ 
				StringTokenizer tokenizer = new StringTokenizer(line_res);
				String id = tokenizer.nextToken();;
				String x_value = tokenizer.nextToken();
				String y_value = tokenizer.nextToken();
				String z_value = tokenizer.nextToken();
				String w_value = tokenizer.nextToken();
				r3id = id;
				r3vl = x_value + " " + y_value + " " + z_value;  //+ " " + w_value;
				String expReg = "r000003vl_" + r3id + ":3double=" + r3vl;
				String data = SIMULID_DATA + "delim" + expReg + "||";
				Event event = EventBuilder.withBody(Bytes.toBytes(data));
			    events.add(event);
				// continue reading lines
				line_res = br_res.readLine();
				countnr++;
				// checking size of events minor than batch size
				if (globalEvents < 998)  
					globalEvents++;
				else {
        			sendFlumeEvent("globalTable.FEM.simulationData.R",events);
        			events.clear();
    				globalEvents = 0;
				}
			}
			// we send in case there is still remaining events
			if (globalEvents > 0){
				sendFlumeEvent("globalTable.FEM.simulationData.R",events);
				events.clear();
			}
			r3nr = countnr;
//		    String expReg = "r3id:" + r3id + "||r3vl:" + r3vl + "||r3nr:" + r3nr;
//			String data = rowkeyValues + "delim" + expReg; 
//			sendFlumeEvent("globalTable.FEM.simulationData.R", Bytes.toBytes(data));
//			sendFlumeEvent("globalTable.FEM.simulationData.R", events);
		}
	}

}	