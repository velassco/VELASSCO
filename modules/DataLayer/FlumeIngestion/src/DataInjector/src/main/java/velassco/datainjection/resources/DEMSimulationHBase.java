package velassco.datainjection.resources;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;

import org.apache.commons.lang.ArrayUtils;
import org.apache.hadoop.hbase.util.Bytes;

import velassco.datainjection.util.HBaseManagerBinary;
import ParticlesAndContacts.Contact;
import ParticlesAndContacts.Particle;
import StaticMesh.Connectivity;
import StaticMesh.MeshPoint;

public class DEMSimulationHBase {

	public static void main(String[] args) {
		/* ingestion parameters */
		String table_name = "Simulations_Data";
		String simulation_name = "Fluidized Bed"; /* information not stored in HBase table*/
		final long simulation_id = 1; /* 1st part of the rowkey */
		final long mesh_id = 333; /* 1st part of the rowkey */
		String analysis_name = "time    "; /* 2 part of the rowkey*/
		final int number_of_partition_for_each_time_step = 1;
		
		String[] families = {"M", "R"};
		
		/* file paths for data simulation and static mesh */
		String path_particles = "/home/giuseppe/Desktop/VELaSCCo/Data/Fluidized_Bed_Small/FluidizedBed_small.p3p";
		String path_contacts = 	"/home/giuseppe/Desktop/VELaSCCo/Data/Fluidized_Bed_Small/FluidizedBed_small.p3c";
		String path_wall_contacts = "/home/giuseppe/Desktop/VELaSCCo/Data/Fluidized_Bed_Small/FluidizedBed_small.p3w";
		String path_static_mesh = "/home/giuseppe/Desktop/VELaSCCo/Data/Fluidized_Bed_Small/D2C_Fluidizedbed_small.post.msh";	
				
		
		try {
			HBaseManagerBinary.creatTable("Simulations_Vquery_Results", families);
		} catch (Exception e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		System.exit(0);		
		
		
		HashMap<Long, List<Particle>> particles = createParticlesFromFile(path_particles);
		HashMap<Long, List<Contact>> contacts = createP2PContactsFromFile(path_contacts);
		HashMap<Long, List<Contact>> wall_contacts = createP2PContactsFromFile(path_wall_contacts);
		HashMap<Long, MeshPoint> mesh_points = createMeshPointsFromFile(path_static_mesh);
		HashMap<Long, Connectivity> mesh_connectivity = createMeshConnectivityFromFile(path_static_mesh);
	
		
		/*
		System.out.println(contacts.size());
		System.out.println(contacts.get((long)2799000).size());
		*/
		
		//System.exit(0);
		System.out.println("Starting data ingestion....");
		//create table if it does not exist
		
		
		
		
		try {
			HBaseManagerBinary.creatTable(table_name, families);
			ingest_p3p_data(table_name, simulation_id, analysis_name, families, particles);
			ingest_p3c_data(table_name, simulation_id, analysis_name, families, contacts);
			ingest_p3w_data(table_name, simulation_id, analysis_name, families, wall_contacts);
			
			//INGESTING STATIC MESH
			ingest_mesh_points_data(table_name, simulation_id,"        ", families, mesh_points);
			ingest_mesh_connectivity_data(table_name, simulation_id,"        ", families, mesh_connectivity);
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public static HashMap<Long,Connectivity> createMeshConnectivityFromFile(String file){
		System.out.println("Creating mesh connecitivities object list from file....");
		HashMap <Long, Connectivity> mesh_connectivities= new HashMap<Long, Connectivity>();
		int line = 1;
		
		try {
		    FileInputStream fstream = new FileInputStream(file);
		    InputStreamReader in = new InputStreamReader(fstream);
		    BufferedReader br = new BufferedReader(in);
		    String strLine;         
		 
		    // Read File Line By Line
		    while (!(strLine = br.readLine()).equals("End Elements")) {
		   	}
		  
		    while (!(strLine = br.readLine()).equals("End Elements")) {
		    	if((strLine.split(" ")[0].equals("MESH") || strLine.split(" ").equals("Coordinates") ||  strLine.equals("End Coordinates") || strLine.equals("Elements"))){
		    		strLine = br.readLine(); 
		    	}
		   
		    	else{
		    		String[] split = strLine.split(" ");
		    		mesh_connectivities.put(Long.valueOf(split[0]), new Connectivity(Long.valueOf(split[0]), new long[] {Long.valueOf(split[1]),  Long.valueOf(split[2]), Long.valueOf(split[3]), Long.valueOf(split[4])}));
		    	}
		    }
		    // Close the input stream
		    in.close();
		    
		} catch (Exception e) {// Catch exception if any
		    System.err.println("ERROR in line " + e.getMessage());
		}
		return mesh_connectivities;
	}
	
	
	public static HashMap<Long,MeshPoint> createMeshPointsFromFile(String file){
		System.out.println("Creating mesh points object list from file....");
		HashMap <Long, MeshPoint> mesh_points= new HashMap<Long, MeshPoint>();
		
		try {
		    FileInputStream fstream = new FileInputStream(file);
		    InputStreamReader in = new InputStreamReader(fstream);
		    BufferedReader br = new BufferedReader(in);
		    String strLine;         
		 
		    // Read File Line By Line
		    while (!(strLine = br.readLine()).equals("End Coordinates")) {
		    	
		    	if((strLine.split(" ")[0].equals("MESH") || strLine.split(" ").equals("Coordinates"))){
		    		strLine = br.readLine(); 
		    	}
		    	else{
		    		String[] split = strLine.split(" ");
		    		mesh_points.put(Long.valueOf(split[0]), new MeshPoint(Long.valueOf(split[0]), new double[] {Double.parseDouble(split[1]),  Double.parseDouble(split[2]), Double.parseDouble(split[3])}));
		    	}
		    }
		    // Close the input stream
		    in.close();
		    
		} catch (Exception e) {// Catch exception if any
		    System.err.println("ERROR in line " + e.getMessage());
		}
		return mesh_points;
	}
	
	
	public static HashMap<Long, List<Contact>> createP2PContactsFromFile(String file){
		System.out.println("Creating contacts object list from file....");
		HashMap<Long, List<Contact>> contacts = new HashMap<Long, List<Contact>>();
		long current_time_step = 0; //current time step
		int line = 1;
		
		try {
		    FileInputStream fstream = new FileInputStream(file);
		    InputStreamReader in = new InputStreamReader(fstream);
		    BufferedReader br = new BufferedReader(in);
		    String strLine;         
		 
		    // Read File Line By Line
		    while ((strLine = br.readLine()) != null) {
		    	if(strLine.equals("TIMESTEP CONTACTS")){ //we are reading a new timestep
		    		
		    		//if (line > 5) //to ingest only the first time step
		    			//break;
		    		
		    		strLine = br.readLine(); // we are reading the line where there is the time step
		    		String[] split = strLine.split(" ");
		    		current_time_step = Long.valueOf(split[0]).longValue();
		    		contacts.put(current_time_step, new ArrayList<Contact>());
		    		br.readLine();
		    		line = 1;
		    	}
		    	
		    	else{
		    		String[] split = strLine.split(" ");
		    		contacts.get(current_time_step).add(new Contact(Long.valueOf(line).longValue(), new long[]{Long.valueOf(split[0]).longValue(),Long.valueOf(split[1]).longValue()}, new double[]{Double.parseDouble(split[2]),Double.parseDouble(split[3]),Double.parseDouble(split[4])},new double[]{Double.parseDouble(split[5]),Double.parseDouble(split[6]),Double.parseDouble(split[7])}));
				    line++;  
		    	}
		    
		    }
		    // Close the input stream
		    in.close();
		    
		} catch (Exception e) {// Catch exception if any
		    System.err.println("ERROR in line "+ line + " " + e.getMessage());
		}
		return contacts;
	}
	
	public static HashMap<Long, List<Particle>> createParticlesFromFile(String file){
		System.out.println("Creating particles object list from file....");
		
		HashMap<Long, List<Particle>> particles = new HashMap<Long, List<Particle>>();
		long current_time_step = 0; //current time step
		int line = 1;
		
		try {
		    FileInputStream fstream = new FileInputStream(file);
		    InputStreamReader in = new InputStreamReader(fstream);
		    BufferedReader br = new BufferedReader(in);
		    String strLine;         
		 
		    // Read File Line By Line
		    while ((strLine = br.readLine()) != null) {
		    	if(strLine.equals("TIMESTEP PARTICLES")){ //we are reading a new timestep
		    		
		    		//if (line > 5) //to ingest only the first time step
		    		//	break;
		    		
		    		strLine = br.readLine(); // we are reading the line where there is the time step
		    		line++;
		    		String[] split = strLine.split(" ");
		    		current_time_step = Long.valueOf(split[0]).longValue();
		    		particles.put(current_time_step, new ArrayList<Particle>());
		    	//	System.out.println("added time step "+ current_time_step);
		    		br.readLine();
		    		line++;
		    	}
		    	
		    	else{
		    		String[] split = strLine.split(" ");
		    		particles.get(current_time_step).add(new Particle(Long.valueOf(split[0]).longValue(), Double.parseDouble(split[3]), Double.parseDouble(split[4]), new double[]{Double.parseDouble(split[5]),Double.parseDouble(split[6]),Double.parseDouble(split[7])},new double[]{Double.parseDouble(split[8]),Double.parseDouble(split[9]),Double.parseDouble(split[10])}));
		    		line++;
		    	}
		    	
		    }
		    // Close the input stream
		    in.close();
		    
		} catch (Exception e) {// Catch exception if any
		    System.err.println("ERROR in line "+ line + " " + e.getMessage());
		}
		
		return particles;
	}
	
	static void ingest_mesh_points_data(String table_name, long simulation_id, String analysis_name, String[] families, HashMap<Long, MeshPoint> mesh_points) throws Exception{
		System.out.println("Ingesting Static mesh points data.....");
		ByteBuffer rowkey = ByteBuffer.allocate(32);
		ByteBuffer coordinate_label = ByteBuffer.allocate(16);
		ByteBuffer coordinate_values = ByteBuffer.allocate(24);
		
		// create the rowkey
		rowkey.put(Bytes.toBytes(simulation_id));
		rowkey.put(analysis_name.getBytes());
		rowkey.put(Bytes.toBytes((long)0));
		rowkey.put(Bytes.toBytes((long)1));
	
		for (Entry<Long, MeshPoint> entry : mesh_points.entrySet()) {
			
			coordinate_label.clear();
			coordinate_values.clear();
			
			System.out.println("Ingesting mesh points data "+ entry.getKey());
			
			coordinate_label.put("c000001_".getBytes());
			coordinate_label.put(Bytes.toBytes(entry.getValue().getId()));
				
	
			coordinate_values.put(Bytes.toBytes(entry.getValue().getCoordinates()[0]));
			coordinate_values.put(Bytes.toBytes(entry.getValue().getCoordinates()[1]));
			coordinate_values.put(Bytes.toBytes(entry.getValue().getCoordinates()[2]));
				
			HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[0], coordinate_label.array(), coordinate_values.array());
				
			}

	}

	static void ingest_mesh_connectivity_data(String table_name, long simulation_id, String analysis_name, String[] families, HashMap<Long, Connectivity> mesh_connectivities) throws Exception{
		System.out.println("Ingesting Static mesh connectivity data.....");
		ByteBuffer rowkey = ByteBuffer.allocate(32);
		ByteBuffer coordinate_label = ByteBuffer.allocate(16);
		ByteBuffer connectivities_values = ByteBuffer.allocate(32);
		
		// create the rowkey
		rowkey.put(Bytes.toBytes(simulation_id));
		rowkey.put(analysis_name.getBytes());
		rowkey.put(Bytes.toBytes((long)0));
		rowkey.put(Bytes.toBytes((long)1));
	
		for (Entry<Long, Connectivity> entry : mesh_connectivities.entrySet()) {
			
			coordinate_label.clear();
			connectivities_values.clear();
			
			System.out.println("Ingesting mesh connectivity data "+ entry.getKey());
			
			coordinate_label.put("m000001_".getBytes());
			coordinate_label.put(Bytes.toBytes(entry.getValue().getId()));
			
			
			connectivities_values.put(Bytes.toBytes(entry.getValue().getConnectivity()[0]));
			connectivities_values.put(Bytes.toBytes(entry.getValue().getConnectivity()[1]));
			connectivities_values.put(Bytes.toBytes(entry.getValue().getConnectivity()[2]));
			connectivities_values.put(Bytes.toBytes(entry.getValue().getConnectivity()[3]));
			
			HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[0], coordinate_label.array(), connectivities_values.array());
				
			}

	}
	
	
	static void ingest_p3w_data(String table_name, long simulation_id, String analysis_name, String[] families, HashMap<Long, List<Contact>> contacts) throws Exception{
		System.out.println("Ingesting P3W data.....");
		ByteBuffer rowkey = ByteBuffer.allocate(32);
		ByteBuffer coordinate_label = ByteBuffer.allocate(16);
		ByteBuffer coordinate_values = ByteBuffer.allocate(24);
		ByteBuffer mesh_label = ByteBuffer.allocate(16);
		ByteBuffer mesh_values = ByteBuffer.allocate(16);
		ByteBuffer result_label = ByteBuffer.allocate(16);
		ByteBuffer result_values = ByteBuffer.allocate(24);
	
		for (Entry<Long, List<Contact>> entry : contacts.entrySet()) {
			if (entry.getKey()==2799000 || entry.getKey()==2819000){
				
			// create the rowkey
			rowkey.clear();
			rowkey.put(Bytes.toBytes(simulation_id));
			rowkey.put(analysis_name.getBytes());
			rowkey.put(Bytes.toBytes(entry.getKey()));
			rowkey.put(Bytes.toBytes((long)1));
			
			for(int i = 0; i < entry.getValue().size(); i++){ // each particle of each
				
				coordinate_label.clear();
				coordinate_values.clear();
				mesh_label.clear();
				mesh_values.clear();
				result_label.clear();
				result_values.clear();
				
				System.out.println("Ingesting P3C data time step "+ entry.getKey() + " ingestion number "+i);
				
				coordinate_label.put("c000003_".getBytes());
				coordinate_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
				
				coordinate_values.put(Bytes.toBytes(entry.getValue().get(i).getCoordinates()[0]));
				coordinate_values.put(Bytes.toBytes(entry.getValue().get(i).getCoordinates()[1]));
				coordinate_values.put(Bytes.toBytes(entry.getValue().get(i).getCoordinates()[2]));
				
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[0], coordinate_label.array(), coordinate_values.array());
				
				mesh_label.put("m000003_".getBytes());
				mesh_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
							
				mesh_values.put(Bytes.toBytes(entry.getValue().get(i).getParticle_ids()[0]));
				mesh_values.put(Bytes.toBytes(entry.getValue().get(i).getId()));
							
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[0], mesh_label.array(), mesh_values.array());
				
				result_label.put("r000005_".getBytes());
				result_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
				
				result_values.put(Bytes.toBytes(entry.getValue().get(i).getForce()[0]));
				result_values.put(Bytes.toBytes(entry.getValue().get(i).getForce()[1]));
				result_values.put(Bytes.toBytes(entry.getValue().get(i).getForce()[2]));
				
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[1],	result_label.array(), result_values.array());
			}
			}
		}

	}

	
	static void ingest_p3c_data(String table_name, long simulation_id, String analysis_name, String[] families, HashMap<Long, List<Contact>> contacts) throws Exception{
		
		System.out.println("Ingesting P3C data.....");
		ByteBuffer rowkey = ByteBuffer.allocate(32);
		ByteBuffer coordinate_label = ByteBuffer.allocate(16);
		ByteBuffer coordinate_values = ByteBuffer.allocate(24);
		ByteBuffer mesh_label = ByteBuffer.allocate(16);
		ByteBuffer mesh_values = ByteBuffer.allocate(24);
		ByteBuffer result_label = ByteBuffer.allocate(16);
		ByteBuffer result_values = ByteBuffer.allocate(24);
	
		for (Entry<Long, List<Contact>> entry : contacts.entrySet()) {
			
			if (entry.getKey()==2799000 || entry.getKey()==2819000){
			// create the rowkey
			rowkey.clear();
			rowkey.put(Bytes.toBytes(simulation_id));
			rowkey.put(analysis_name.getBytes());
			rowkey.put(Bytes.toBytes(entry.getKey()));
			rowkey.put(Bytes.toBytes((long)1));
			
			for(int i = 0; i < entry.getValue().size(); i++){ // each particle of each
				
				coordinate_label.clear();
				coordinate_values.clear();
				mesh_label.clear();
				mesh_values.clear();
				result_label.clear();
				result_values.clear();
				
				System.out.println("Ingesting P3C data time step "+ entry.getKey() + " ingestion number "+i);
			
				coordinate_label.put("c000002_".getBytes());
				coordinate_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
				
		
				coordinate_values.put(Bytes.toBytes(entry.getValue().get(i).getCoordinates()[0]));
				coordinate_values.put(Bytes.toBytes(entry.getValue().get(i).getCoordinates()[1]));
				coordinate_values.put(Bytes.toBytes(entry.getValue().get(i).getCoordinates()[2]));
				
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[0], coordinate_label.array(), coordinate_values.array());
				
			
				mesh_label.put("m000002_".getBytes());
				mesh_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
							
			
				mesh_values.put(Bytes.toBytes(entry.getValue().get(i).getParticle_ids()[0]));
				mesh_values.put(Bytes.toBytes(entry.getValue().get(i).getParticle_ids()[1]));
				mesh_values.put(Bytes.toBytes(entry.getValue().get(i).getId()));
							
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[0], mesh_label.array(), mesh_values.array());
				
				result_label.put("r000004_".getBytes());
				result_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
				
				result_values.put(Bytes.toBytes(entry.getValue().get(i).getForce()[0]));
				result_values.put(Bytes.toBytes(entry.getValue().get(i).getForce()[1]));
				result_values.put(Bytes.toBytes(entry.getValue().get(i).getForce()[2]));
					
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[1],	result_label.array(), result_values.array());
				
			}
		}
	}

	}


	
	static void ingest_p3p_data(String table_name, long simulation_id, String analysis_name, String[] families, HashMap<Long, List<Particle>> particles) throws Exception{
		System.out.println("Ingesting P3P data.....");
		
		ByteBuffer rowkey = ByteBuffer.allocate(32);
		ByteBuffer coordinate_label = ByteBuffer.allocate(16);
		ByteBuffer coordinate_values = ByteBuffer.allocate(24);
		ByteBuffer mesh_label = ByteBuffer.allocate(16);
		ByteBuffer result1_label = ByteBuffer.allocate(16);
		ByteBuffer result2_label = ByteBuffer.allocate(16);
		ByteBuffer result3_label = ByteBuffer.allocate(16);
		ByteBuffer result3_values = ByteBuffer.allocate(24);
		
							
		for (Entry<Long, List<Particle>> entry : particles.entrySet()) {
			
			if (entry.getKey()==2799000  || entry.getKey()==2819000){
			
			//System.out.println("NEW ENTRY");
			
			// create the rowkey
			rowkey.clear();
			rowkey.put(Bytes.toBytes(simulation_id));
			rowkey.put(analysis_name.getBytes());
			rowkey.put(Bytes.toBytes(entry.getKey()));
			rowkey.put(Bytes.toBytes((long)1));
			
			System.out.println(entry.getKey() + " " + entry.getValue().size());
			
			for(int i = 0; i < entry.getValue().size(); i++){ // each particle of each
				
				coordinate_label.clear(); 
				coordinate_values.clear();
				mesh_label.clear();
				result1_label.clear();
				result2_label.clear();
				result3_label.clear();
				result3_values.clear();
				
				System.out.println("Ingesting P3P data time step "+ entry.getKey() + " ingestion number "+i);
							
				coordinate_label.put("c000001_".getBytes());
				coordinate_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
							
				coordinate_values.put(Bytes.toBytes(entry.getValue().get(i).getCoordinates()[0]));
				coordinate_values.put(Bytes.toBytes(entry.getValue().get(i).getCoordinates()[1]));
				coordinate_values.put(Bytes.toBytes(entry.getValue().get(i).getCoordinates()[2]));
				
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[0], coordinate_label.array(), coordinate_values.array());
								
				mesh_label.put("m000001_".getBytes());
				mesh_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
				
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[0], mesh_label.array(), Bytes.toBytes(entry.getValue().get(i).getId()));
				
				result1_label.put("r000001_".getBytes());
				result1_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
				
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[1], result1_label.array(), Bytes.toBytes(entry.getValue().get(i).getVolume()));
				
				result2_label.put("r000002_".getBytes());
				result2_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
				
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[1], result2_label.array(), Bytes.toBytes(entry.getValue().get(i).getMass()));
				
				result3_label.put("r000003_".getBytes());
				result3_label.put(Bytes.toBytes(entry.getValue().get(i).getId()));
				
				result3_values.put(Bytes.toBytes(entry.getValue().get(i).getVelocity()[0]));
				result3_values.put(Bytes.toBytes(entry.getValue().get(i).getVelocity()[1]));
				result3_values.put(Bytes.toBytes(entry.getValue().get(i).getVelocity()[2]));
			
				HBaseManagerBinary.addBytesRecord(table_name, rowkey.array(), families[1], result3_label.array(), result3_values.array());
			}
			
		}

		}
			
	}

}

