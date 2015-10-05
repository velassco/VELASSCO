package StaticMesh;

import java.util.ArrayList;
import java.util.List;
import ParticlesAndContacts.Particle;
import ParticlesAndContacts.Contact;

public class MeshPoint{
	
	long id;
	double[] coordinates = new double[3]; 
		
	public MeshPoint(long a, double[] mesh_coordinates){
	   	id = a;
	   	coordinates = mesh_coordinates;
	}
	
	public long getId(){
		return id;
	}

	public double[] getCoordinates(){
		return coordinates;
	}
	
	
	@Override public String toString() {
		StringBuilder result = new StringBuilder();
		String NEW_LINE = System.getProperty("line.separator");

		result.append(this.getClass().getName() + " Object {" + NEW_LINE);
		result.append(" ID: " + id + NEW_LINE);
		result.append(" Coords (x,y,x): " + coordinates[0] + "," + coordinates[1] + "," + coordinates[2] + NEW_LINE);
		result.append("}");

		return result.toString();
	 }
}