package velassco.datainjection.bean;

import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlTransient;

import com.google.gson.annotations.SerializedName;


public class Response {
	
	
	/**
	 * {
		Row: [1]
			0:  {
			key: "MTQxMDE5MjI4Mzc1Ny1Ha2JYQXdIWTdWLTc="
			Cell: [3]
			0:  {
			column: "c2ltdWxhdGlvbl9pbmZvOnBhcnRSZXN1bHQ="
			timestamp: 1410192287778
			$: "V2VsY29tZQ=="
			}-
			1:  {
			column: "c2ltdWxhdGlvbl9pbmZvOnJhd19kYXRh"
			timestamp: 1410192287778
			$: "Rmx1bWUgaXMgYSBkaXN0cmlidXRlZCwgcmVsaWFibGUsIGFuZCBhdmFpbGFibGUgc2VydmljZSBmb3IgZWZmaWNpZW50bHkgY29sbGVjdGluZywgYWdncmVnYXRpbmcsIGFuZCBtb3ZpbmcgbGFyZ2UgYW1vdW50cyBvZiBsb2cgZGF0YS4="
			}-
			2:  {
			column: "c2ltdWxhdGlvbl9pbmZvOnNpbXVsSWQ="
			timestamp: 1410192287778
			$: "MDAx"
			}-
			-
			}-
		-
		}
	 */
	
	@SerializedName("Row")
	//@XmlTransient
    public Row[] rows;

	public static class Row {
		
        @SerializedName("Cell")	
        //@XmlTransient
        private Cell[] cells;
		
		//@XmlTransient
        private String key;

        //@XmlElement
		public Cell[] getCells() {
			return cells;
		}
	
		public void setCells(Cell[] cells) {
			this.cells = cells;
		}
	
		//@XmlElement
		public String getKey() {
			return key;
		}
		
		public void setKey(String key) {
			this.key = key;
		}
    }

    public static class Cell {
        @SerializedName("column")
    	//@XmlTransient
    	private String column;
       
        @SerializedName("timestamp")
    	//@XmlTransient
    	private float timestamp;
       
        @SerializedName("$")
    	//@XmlTransient
    	private String $;

    	//@XmlElement
		public String getColumn() {
			return column;
		}
	
		public void setColumn(String column) {
			this.column = column;
		}
	
		//@XmlElement
		public float getTimestamp() {
			return timestamp;
		}
	
		public void setTimestamp(float timestamp) {
			this.timestamp = timestamp;
		}
	
		//@XmlElement
		public String get$() {
			return $;
		}
	
		public void set$(String $) {
			this.$ = $;
		}    
    }

}
