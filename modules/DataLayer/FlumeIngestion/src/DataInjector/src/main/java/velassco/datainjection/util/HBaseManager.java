package velassco.datainjection.util;

import java.beans.BeanInfo;
import java.beans.Introspector;
import java.beans.PropertyDescriptor;
import java.io.IOException;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;

import javax.ws.rs.core.MediaType;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.HColumnDescriptor;
import org.apache.hadoop.hbase.HTableDescriptor;
import org.apache.hadoop.hbase.KeyValue;
import org.apache.hadoop.hbase.client.Get;
import org.apache.hadoop.hbase.client.HBaseAdmin;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.client.ResultScanner;
import org.apache.hadoop.hbase.client.Scan;
import org.apache.hadoop.hbase.filter.BinaryComparator;
import org.apache.hadoop.hbase.filter.CompareFilter;
import org.apache.hadoop.hbase.filter.CompareFilter.CompareOp;
import org.apache.hadoop.hbase.filter.Filter;
import org.apache.hadoop.hbase.filter.FilterList;
import org.apache.hadoop.hbase.filter.PageFilter;
import org.apache.hadoop.hbase.filter.RowFilter;
import org.apache.hadoop.hbase.filter.SingleColumnValueFilter;
import org.apache.hadoop.hbase.util.Bytes;

import com.sun.jersey.api.client.Client;
import com.sun.jersey.api.client.WebResource;
import com.sun.jersey.api.client.config.DefaultClientConfig;

public class HBaseManager {

	private static HBaseManager instance = null;
	private static Configuration hBaseConfig;
	private String velasscoTableName;
	private List<String> hbaseColumns = new ArrayList<String>();
	private HashMap<String, ArrayList<String>> columnMappings = new HashMap<String, ArrayList<String>>(); 
	private HashMap<String, String> qualifierMappings = new HashMap<String, String>();
	
	protected HBaseManager() {
		Properties properties = new Properties();
		try {
			hBaseConfig =  HBaseConfiguration.create();
		    hBaseConfig.setInt("timeout", 50);
//		    hBaseConfig.set("hbase.master", "*212.170.156.84:9000*");
		    hBaseConfig.set("hbase.master", "*192.168.215.61:9000*");
//		    hBaseConfig.set("hbase.zookeeper.quorum", "212.170.156.84");
		    hBaseConfig.set("hbase.zookeeper.quorum", "192.168.215.61");
		    hBaseConfig.set("hbase.zookeeper.property.clientPort", "2181");
		    hBaseConfig.set("hbase.client.keyvalue.maxsize","50");
			
			properties.load(this.getClass().getResourceAsStream("/hbase.properties"));
			Enumeration<String> itProp = (Enumeration<String>) properties.propertyNames();
			while (itProp.hasMoreElements()) {
				String propName = itProp.nextElement();
				if (propName.equalsIgnoreCase("hbase.table")) {
					velasscoTableName = properties.getProperty("hbase.table");
				} else {
					String colFamily = properties.getProperty(propName);
					String qualifier = propName.substring(propName.indexOf(".")+1);	
					ArrayList<String> values = columnMappings.get(colFamily);
					if (values==null) {
						values = new ArrayList<String>();
					}									
					values.add(qualifier);
					//System.out.println(" ### MAPPINGS : key = " + properties.getProperty(propName) + " value = " + values.toString());
					columnMappings.put(colFamily, values);
					qualifierMappings.put(qualifier, colFamily);
				}
			}

			loadTableDefinition();
			
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static HBaseManager getInstance() {
	  if(instance == null) {
	     instance = new HBaseManager();         
	  }
	  return instance;
	}
	
	
	private void loadTableDefinition () throws IOException { 
	  	 
	  HTable table = new HTable(hBaseConfig, this.velasscoTableName);	
	  Iterator<HColumnDescriptor> it = table.getTableDescriptor().getFamilies().iterator();
	  while (it.hasNext()) {
		  HColumnDescriptor cDesc = it.next();
		  this.hbaseColumns.add(cDesc.getNameAsString());
		  //System.out.println(" ############# COLUMN: " + cDesc.getNameAsString());
	  }
	  
	}
	
	public String queryTable (String rowkey, String database) throws IOException {  
		Client client = Client.create(new DefaultClientConfig());
		WebResource webResource = client.resource("http://192.168.215.61:8000").path(database);
		String response = ""; 
		try {
			response =  webResource.path(rowkey).accept("application/json").get(String.class);
    	} catch (Exception e) {
    		e.printStackTrace();
    	}
		client.destroy();
		System.out.println(response);
		return response;
	}
	
	
	public String createTable(String tableName, List<String> columns) throws IOException{
		
		String response = ""; 
		HBaseConfiguration conf = new HBaseConfiguration();
		conf.set("hbase.master", "*212.170.156.84:9000*");
		 
		HBaseAdmin hbase = new HBaseAdmin(conf);
		HTableDescriptor desc = new HTableDescriptor(tableName);
		for (String column : columns){
			HColumnDescriptor col = new HColumnDescriptor(column.getBytes());
			desc.addFamily(col);
		}
		hbase.createTable(desc);
		
		return response;
	}
	
		
}
