package ParticlesAndContacts;

public class Contact {

	long id;
	long[] particle_ids = new long[2];
	double[] coordinates = new double[3]; 
	double[] force = new double[3]; 

	public Contact(long c_id, long[] p_ids, double[] c_coords, double[] c_force){
	   	id = c_id;
	   	particle_ids = p_ids;
	   	coordinates = c_coords;
	   	force = c_force;
	}
	
	public long getId() {
		return id;
	}

	public long[] getParticle_ids() {
		return particle_ids;
	}

	public void setParticle_ids(long[] particle_ids) {
		this.particle_ids = particle_ids;
	}

	public double[] getCoordinates() {
		return coordinates;
	}

	public void setCoordinates(double[] coordinates) {
		this.coordinates = coordinates;
	}

	public double[] getForce() {
		return force;
	}

	public void setForce(double[] force) {
		this.force = force;
	}

	public void setId(long id) {
		this.id = id;
	}
	
	@Override public String toString() {
		StringBuilder result = new StringBuilder();
		String NEW_LINE = System.getProperty("line.separator");
		result.append(this.getClass().getName() + " Object {" + NEW_LINE);
		result.append(" ID: " + id + NEW_LINE);
		result.append(" Paticle IDS: " + particle_ids[0] + "," + particle_ids[1] + NEW_LINE);
		result.append(" Coords (x,y,z): " + coordinates[0] + "," + coordinates[1] + "," + coordinates[2] + NEW_LINE);
		result.append(" Force (x,y,z): " + force[0] + "," + force[1] + "," + force[2] + NEW_LINE);
		result.append("}");

		return result.toString();
	 }
};