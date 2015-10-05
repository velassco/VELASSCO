package velassco.datainjection.bean;

import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlTransient;

@XmlRootElement
public class OutputPartXML {
	
	// structure of output XML
	@XmlTransient
	private String simulId;

	public String getSimulId() {
		return simulId;
	}

	public void setSimulId(String simulId) {
		this.simulId = simulId;
	}
	
	
	@XmlTransient
	private String partNumber;

	public String getPartNumber() {
		return partNumber;
	}

	public void setPartNumber(String partNumber) {
		this.partNumber = partNumber;
	}
	
	@XmlTransient
	private String sim_raw_data;

	public String getRaw_data() {
		return sim_raw_data;
	}

	public void setRaw_data(String raw_data) {
		this.sim_raw_data = raw_data;
	}

	
}
