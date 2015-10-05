package ParticlesAndContacts;

public class Particle {
	
	long id;
	double[] coordinates = new double[3];
	double volume, mass;
	double[] velocity = new double[3];
	
	public Particle(){
		id = 0;
		coordinates[0] = coordinates[1] = coordinates[2] =  0;
		volume = 0;
		mass = 0;
		velocity[0] = velocity[1] = velocity[2] = 0;
	}
	
	public Particle(long p_id, double p_volume, double p_mass, double[] p_coords, double[] p_vel){
	   	id = p_id;
	   	coordinates = p_coords;
	   	volume = p_volume;	   	
	   	mass = p_mass;
	   	velocity = p_vel;
	}
	
	public long getId() {
		return id;
	}

	public void setId(long id) {
		this.id = id;
	}

	public double[] getCoordinates() {
		return coordinates;
	}

	public void setCoordinates(double[] coordinates) {
		this.coordinates = coordinates;
	}

	public double getVolume() {
		return volume;
	}

	public void setVolume(double volume) {
		this.volume = volume;
	}

	public double getMass() {
		return mass;
	}

	public void setMass(double mass) {
		this.mass = mass;
	}

	public double[] getVelocity() {
		return velocity;
	}

	public void setVelocity(double[] velocity) {
		this.velocity = velocity;
	}

	@Override public String toString() {
		StringBuilder result = new StringBuilder();
		String NEW_LINE = System.getProperty("line.separator");

		result.append(this.getClass().getName() + " Object {" + NEW_LINE);
		result.append(" ID: " + id + NEW_LINE);
		result.append(" Coords (x,y,x): " + coordinates[0] + "," + coordinates[1] + "," + coordinates[2] + NEW_LINE);
		result.append(" Volume: " + volume + NEW_LINE);
		result.append(" Mass: " + mass + NEW_LINE );
		result.append(" Velocity (x,y,x): " + velocity[0] + "," + velocity[1] + "," + velocity[2] + NEW_LINE);
		result.append("}");

		return result.toString();
	 }
};
	