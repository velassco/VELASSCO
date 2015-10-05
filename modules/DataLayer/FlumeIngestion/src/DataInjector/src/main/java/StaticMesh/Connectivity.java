package StaticMesh;

import java.util.ArrayList;
import java.util.List;
import ParticlesAndContacts.Particle;
import ParticlesAndContacts.Contact;

public class Connectivity{
	
	long id;
	long[] connectivity_points = new long[4]; 
		
	public Connectivity(long a, long[] mesh_points){
	   	id = a;
	   	connectivity_points = mesh_points;
	}
	
	public long getId(){
		return id;
	}

	public long[] getConnectivity(){
		return connectivity_points;
	}
	
	
	@Override public String toString() {
		StringBuilder result = new StringBuilder();
		String NEW_LINE = System.getProperty("line.separator");

		result.append(this.getClass().getName() + " Object {" + NEW_LINE);
		result.append(" ID: " + id + NEW_LINE);
		result.append(" Coonectivity points: " + connectivity_points[0] + "," + connectivity_points[1] + "," + connectivity_points[2] + "," + connectivity_points[3] + NEW_LINE);
		result.append("}");

		return result.toString();
	 }
}