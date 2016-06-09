package velassco.datainjection.bean;

import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlTransient;


@XmlRootElement
public class ResponseXML {
	
	public ResponseXML(){
	}
	
	public ResponseXML(int size){
		this.rows = new RowXML[size];
	}

	@XmlElement
    public RowXML[] rows;

	public static class RowXML {
		
		public RowXML(){
		}
		
		public RowXML(int size){
			this.cells = new CellXML[size];
		}
			
        @XmlTransient
        private CellXML[] cells;
		
		@XmlTransient
        private String key;

        @XmlElement
		public CellXML[] getCells() {
			return cells;
		}
	
		public void setCells(CellXML[] cells) {
			this.cells = cells;
		}
		
		public void setCell(int index, CellXML cell){
			this.cells[index] = cell;
		}
	
		@XmlElement
		public String getKey() {
			return key;
		}
		
		public void setKey(String key) {
			this.key = key;
		}
    }

    public static class CellXML {
    	@XmlTransient
    	private String column;
       
    	@XmlTransient
    	private float timestamp;
       
    	@XmlTransient
    	private String value;

    	@XmlElement
		public String getColumn() {
			return column;
		}
	
		public void setColumn(String column) {
			this.column = column;
		}
	
		@XmlElement
		public float getTimestamp() {
			return timestamp;
		}
	
		public void setTimestamp(float timestamp) {
			this.timestamp = timestamp;
		}
	
		@XmlElement
		public String getValue() {
			return value;
		}
	
		public void setValue(String value) {
			this.value = value;
		}    
    }

}
