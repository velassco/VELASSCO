package velassco.datainjection.resources;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.StringTokenizer;
import java.util.logging.Level;
import java.util.logging.Logger;


public class DEMSimulation extends SimulationManager {
	
//	private static final Logger log = Logger.getLogger(DEMSimulation.class.getName() );
//	
	protected DEMSimulation() throws Exception {
		super();
		// TODO Auto-generated constructor stub
	}
//
//	@Override
//	protected void runETLProcess(String simulationType, String fileName, String partId, String type) throws IOException, InterruptedException{
//		int hashCode = Math.abs(simulationType.hashCode());
//		String simulationID = Integer.toString(hashCode); // 1 put
//		String boundingBox = "empty";
//		String validationStatus = "validated";
//		String numberPart = partId;
//		String otherData = "NULL";
//		// send data
//		// connect to flume manager
//		sendFlumeEvent("globalTable.tableInformation", simulationType + "-" + simulationID  + "-" + boundingBox + "-" + validationStatus  + "-" + numberPart  + "-" + otherData);
//		// DEM simulations for spherical particles	
//		//readFile(simulationName,analysisName,partId);
//	}
//	
//
//	private void readFile(String simulationName, String analysisName, String partId) throws IOException, InterruptedException {
//
//		// reading the file line by line
//		BufferedReader br = null;
//		try {
//	        // *** LOCAL file ***
//	        br = new BufferedReader(new FileReader(NODE_CLUSTER_PATH + simulationName + "." + analysisName));
//			// ********************
//	        // readHDFSFile();
//		} catch (Exception ex){
//			log.log(Level.SEVERE, ex.toString(), ex );
//		}
//			
//	    int count=1;
//		try {
//	        String line = br.readLine();
//	        // file parameters
//	        String timestep = "true";
//	        String contacts = "false";
//	        String particles = "false";
//	        int numberTimeSteps = 0;
//	        //append to file
//	        while (line != null) {
//	        	// 1st line
//	        	if (line.contains("TIMESTEP")){
//	        		if (line.contains("CONTACTS")) {
//		        		// 2nd line
//		        		line = br.readLine();
//		        		StringTokenizer tokenizer = new StringTokenizer(line);
//	    				timestep = tokenizer.nextToken();
//	    				contacts = tokenizer.nextToken();
//	    				numberTimeSteps++;
//	    				
//	    				// generating Keys
//	    				String key = analysisName + timestep;
//	    				String keyData = analysisName + timestep + partId;
//	    				
//	    				// metadata
//	    				if (analysisName.equals("p3c")){
//	    					sendFlumeEvent("metadata.CoordinatesP3C",key + " - " + contacts);
//		    				sendFlumeEvent("metadata.GaussPoint1", key + " - p2p gp - line - 1 - false - true - empty - p2p");
//	    				} else if (analysisName.equals("p3w")) {
//	    					sendFlumeEvent("metadata.CoordinatesP3W", key + " - " + contacts);
//		    				sendFlumeEvent("metadata.GaussPoint2", key + " - p2w gp - line - 1 - false - true - empty - p2w");
//	    				}
//	    				
//	    				// 3rd line
//	    				line = br.readLine();
//	    				
//	    				// 4th line to ahead
//	    				for (int i=0; i< Integer.parseInt(contacts); i++){
//	    					line = br.readLine();
//	    					if (analysisName.equals("p3c")){
//	    						insertP3CCoordinates(line,timestep,key,keyData,i);
//	    					} else if (analysisName.equals("p3w")) {
//	    						insertP3WCoordinates(line,timestep,key,keyData,i);
//	    					}
//	    				}	
//	    				
//	        		} else {
//		        		// 2nd line
//		        		line = br.readLine();
//		        		StringTokenizer tokenizer = new StringTokenizer(line);
//	    				timestep = tokenizer.nextToken();
//	    				particles = tokenizer.nextToken();
//	    				numberTimeSteps++;
//	    				
//	    				// generating Keys
//	    				String key = analysisName + timestep;
//	    				String keyData = analysisName + timestep + partId;
//	    				
//	    				// metadata
//	    				sendFlumeEvent("metadata.CoordinatesP3P", key + " - " + particles);
//	    				sendFlumeEvent("metadata.Mesh1", key + " - Particles - Sphere - " + particles + " - 1 - Not provided - Not provided");	
//	    				
//	    				// 3rd line
//	    				line = br.readLine();
//	    				
//	    				// 4th line to ahead
//	    				for (int i=0; i< Integer.parseInt(particles); i++){
//	    					line = br.readLine();
//    						insertP3PCoordinates(line,timestep,key,keyData,i);
//	    				}
//	        		}
//	        	}	
//			    // continue reading lines
//		        line = br.readLine();
//		        // increment counter
//		        count++;
//	        }
//	        sendFlumeEvent("globalTable.numberTimeSteps", simulationName + "-" + numberTimeSteps);
//
//	    } finally {
//	        br.close();
//	        log.log(Level.FINE, "closing simulation data file");
//	    }
//	}
//
//	private void insertP3PCoordinates(String line, String timestep, String key, String keyData, int index) {
//		// TODO Auto-generated method stub
//		String[] values = line.split("\\ ");
//		String id = values[0];
//		String group = values[1];
//		String volume = values[3];
//		String mass = values[4];
//		String px = values[5];
//		String py = values[6];
//		String pz = values[7];
//		String velocity = values[8] + ", " + values[9] + ", " + values[10];
//		sendFlumeEvent("simulationdata.CoordinatesDataP3P", keyData + "-" + index + " # " + id + " # " + px + " # " + py + " # " +  pz);
//		
//		// metadata
//		sendFlumeEvent("metadata.Result1", key + "-" + index + " # " + volume + " # Scalar # 1 # empty # OnNodes # empty # empty");
//		sendFlumeEvent("metadata.Result2", key + "-" + index + " # " + mass + " # Scalar # 1 # empty # OnNodes # empty # empty");
//		sendFlumeEvent("metadata.Result3", key + "-" + index + " # " + velocity + " # Vector # 3 # {Velocity X, Velocity Y, Velocity Z} # OnNodes # empty # empty");
//		
//		// data
//		String idMeshData = Integer.toString(Math.abs((timestep + "mesh1").hashCode()));
//		double radius = 0.0;
//		try {
//			radius = Math.pow( ( (3.0 * Double.parseDouble(volume)) /(4 * Math.PI) ), 1.0/3.0); //Radius = ((3*VOLUME)/(4*PI))^(1/3)
//
//		} catch (Exception e) {
//			System.out.println(e);
//		}
//		sendFlumeEvent("simulationdata.MeshData1", keyData + "-" + index + " # " +  idMeshData + " # " + id + " # " + Double.toString(radius) + " # " +  group);
//		sendFlumeEvent("simulationdata.ResultData1", keyData + "-" + index + " # " + id + " # " + volume);
//		sendFlumeEvent("simulationdata.ResultData2", keyData + "-" + index + " # " + id + " # " + mass);
//		sendFlumeEvent("simulationdata.ResultData3", keyData + "-" + index + " # " + id + " # " + values[8] + " # " + values[9] + " # " + values[10]);
//	}
//
//	private void insertP3WCoordinates(String line, String timestep, String key, String keyData, int index) {
//		// TODO Auto-generated method stub
//		String[] values = line.split("\\ ");
//		String forcep2w = values[5] + ", " + values[6] + ", " + values[7];
//		String cx = values[2];
//		String cy = values[3];
//		String cz = values[4];
//		String wall = values[1];
//		String p1 = values[0];
//		
//		// metadata
//		String metadataR5 = key + "-" + index + " # " + forcep2w + " # Vector # 3 # {Force p2w X, Force p2w Y, Force p2w Z} # OnGaussPoints # p2w gp # empty";
//		sendFlumeEvent("metadata.Result5", metadataR5);
//		
//		// data
//		String idDataP3W = Integer.toString(Math.abs(timestep.hashCode()));
//		sendFlumeEvent("simulationdata.CoordinatesDataP3W", keyData + "-" + index + " # " + idDataP3W + " # " + cx + " # " + cy + " # " + cz);
//		String idMeshData3 = Integer.toString(Math.abs((timestep + "mesh3").hashCode()));
//		sendFlumeEvent("simulationdata.MeshData3", keyData + "-" + index + " # " +  idMeshData3 + " # " + p1 + "  # " + idDataP3W + " # " + wall);	
//		String dataR5 = keyData + "-" + index + " # " + idMeshData3 + " # " + values[5] + " # " + values[6] + " # " + values[7];
//		sendFlumeEvent("simulationdata.ResultData5", dataR5);
//	}
//
//	private void insertP3CCoordinates(String line, String timestep, String key, String keyData, int index) {
//		// TODO Auto-generated method stub
//		String[] values = line.split("\\ ");
//		String p1 = values[0]; 
//		String p2 = values[1];	    								
//		String cx = values[2];
//		String cy = values[3];
//		String cz = values[4];
//		String forcep2p = values[5] + ", " + values[6] + ", " + values[7];
//		
//		// metadata
//		sendFlumeEvent("metadata.Result4", key + "-" + index + " # " + forcep2p + " # Vector # 3 # {Force p2p X, Force p2p Y, Force p2p Z} # OnGaussPoints # p2p gp # empty");
//		
//		// data
//		String idDataP3C = Integer.toString(Math.abs(timestep.hashCode()));
//		sendFlumeEvent("simulationdata.CoordinatesDataP3C", keyData + "-" +  index + " # " + idDataP3C + " # " + cx + " # " + cy + " # " + cz);
//		String idMeshData2 = Integer.toString(Math.abs((timestep + "mesh2").hashCode()));
//		sendFlumeEvent("simulationdata.MeshData2",keyData + "-" + index + " # " +  idMeshData2 + " # " + p1 + " # " + p2 + " # " + idDataP3C + " # " + "empty");
//		sendFlumeEvent("simulationdata.ResultData4",keyData + "-" + index + " # " + idMeshData2 + " # " + values[5] + " # " + values[6] + " # " + values[7]);
//	}

}
