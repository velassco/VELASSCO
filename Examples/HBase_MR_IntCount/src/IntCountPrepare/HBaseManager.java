import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.HColumnDescriptor;
import org.apache.hadoop.hbase.HTableDescriptor;
import org.apache.hadoop.hbase.KeyValue;
import org.apache.hadoop.hbase.MasterNotRunningException;
import org.apache.hadoop.hbase.ZooKeeperConnectionException;
import org.apache.hadoop.hbase.client.Delete;
import org.apache.hadoop.hbase.client.Get;
import org.apache.hadoop.hbase.client.HBaseAdmin;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.client.ResultScanner;
import org.apache.hadoop.hbase.client.Scan;
import org.apache.hadoop.hbase.util.Bytes;

public class HBaseManager {
    static Configuration conf = null;
    /**
     * Initialization
     */ 
    static {  
	conf = HBaseConfiguration.create();  
    }  
       
    /**   
     * Create a new table   
     */    
    public static Configuration getConfiguration() {
	return conf;
    }

    public static boolean creatTable(String tableName, String[] familys) throws Exception {
	@SuppressWarnings("resource")
        HBaseAdmin admin = new HBaseAdmin(conf);     
        if (admin.tableExists(tableName)) {     
	    System.out.println("table already exists!");
	    return false;
        } else {
            HTableDescriptor tableDesc = new HTableDescriptor(tableName);     
            for(int i=0; i<familys.length; i++){     
            	tableDesc.addFamily(new HColumnDescriptor(familys[i]));     
            }  
            admin.createTable(tableDesc);     
            System.out.println("create table " + tableName + " ok.");     
	    return true;
        }
    }     

    public static void deleteTable(String tableName) throws Exception {     
	try {     
	    @SuppressWarnings("resource")
	    HBaseAdmin admin = new HBaseAdmin(conf);     
	    admin.disableTable(tableName);     
	    admin.deleteTable(tableName);     
	    System.out.println("delete table " + tableName + " ok.");     
	} catch (MasterNotRunningException e) {     
	    e.printStackTrace();     
	} catch (ZooKeeperConnectionException e) {     
	    e.printStackTrace();     
	}
    }      

    public static void addRecord (String tableName, String rowKey, String family, String qualifier, String value)     
	throws Exception{     
        try {     
            HTable table = new HTable(conf, tableName);     
            Put put = new Put(Bytes.toBytes(rowKey));     
            put.add(Bytes.toBytes(family),Bytes.toBytes(qualifier),Bytes.toBytes(value));     
            table.put(put);     
            System.out.println("insert recored " + rowKey + " to table " + tableName +" ok.");     
	    table.close();
        } catch (IOException e) {     
            e.printStackTrace();     
        }     
    }

    public static void addRecord (String tableName, String rowKey, String family, String qualifier, Double value)     
	throws Exception{     
        try {     
            HTable table = new HTable(conf, tableName);     
            Put put = new Put(Bytes.toBytes(rowKey));     
            put.add(Bytes.toBytes(family),Bytes.toBytes(qualifier),Bytes.toBytes(value));     
            table.put(put);     
            System.out.println("insert recored " + rowKey + " to table " + tableName +" ok.");     
	    table.close();
        } catch (IOException e) {     
            e.printStackTrace();     
        }
    }
    
    public static void addBytesRecord (String tableName, String rowKey, String family, String qualifier, byte[] value)     
	throws Exception{     
        try {     
            HTable table = new HTable(conf, tableName);     
            Put put = new Put(Bytes.toBytes(rowKey));     
            put.add(Bytes.toBytes(family),Bytes.toBytes(qualifier),value);     
            table.put(put);     
	    //  System.out.println("insert recored " + rowKey + " to table " + tableName +" ok.");     
	    table.close();
        } catch (IOException e) {     
            e.printStackTrace();     
        }
    }
    
    public static void addBytesRecord (String tableName, byte[] rowKey, String family, byte[] qualifier, byte[] value)     
	throws Exception{     
        try {     
            HTable table = new HTable(conf, tableName);     
            Put put = new Put(rowKey);     
            put.add(Bytes.toBytes(family),qualifier,value);     
            table.put(put);     
	    //  System.out.println("insert recored " + rowKey + " to table " + tableName +" ok.");     
	    table.close();
        } catch (IOException e) {     
            e.printStackTrace();     
        }
    }
    
    public static void addBytesRecord (String tableName, String rowKey, String family, byte[] qualifier, byte[] value)     
	throws Exception{     
        try {     
            HTable table = new HTable(conf, tableName);     
            Put put = new Put(Bytes.toBytes(rowKey));     
            put.add(Bytes.toBytes(family),qualifier,value);     
            table.put(put);     
            //System.out.println("insert recored " + rowKey + " to table " + tableName +" ok.");     
	    table.close();
        } catch (IOException e) {     
            e.printStackTrace();     
        }
    } 

    public static void addIntegerRecord (String tableName, String rowKey, String family, String qualifier, int value)     
	throws Exception{     
        try {     
            HTable table = new HTable(conf, tableName);     
            Put put = new Put(Bytes.toBytes(rowKey));     
            
            put.add(Bytes.toBytes(family),Bytes.toBytes(qualifier),Bytes.toBytes(value));     
            table.put(put);     
            System.out.println("insert recored " + rowKey + " to table " + tableName +" ok.");     
	    table.close();
        } catch (IOException e) {     
            e.printStackTrace();     
        }
    } 

    public static void delRecord (String tableName, String rowKey) throws IOException{     
        HTable table = new HTable(conf, tableName);     
	List<Delete> list = new ArrayList<Delete>();
        Delete del = new Delete(rowKey.getBytes());     
        list.add(del);     
        table.delete(list);     
        System.out.println("del recored " + rowKey + " ok.");     
	table.close();
    }     

    public static Result getColumnQualifier (String tableName, String rowKey, String family, String columnQualifier) throws IOException{
	HTable table = new HTable(conf, tableName);     
	Get get = new Get(rowKey.getBytes());     
	get.addColumn(family.getBytes(), columnQualifier.getBytes());
	return table.get(get);     
    }
    
    public static void getOneRecord (String tableName, String rowKey) throws IOException{     
        HTable table = new HTable(conf, tableName);     
        Get get = new Get(rowKey.getBytes());     
        Result rs = table.get(get);     
        for(KeyValue kv : rs.raw()){     
            System.out.print(new String(kv.getRow()) + " " );     
            System.out.print(new String(kv.getFamily()) + ":" );     
            System.out.print(new String(kv.getQualifier()) + " " );     
            System.out.print(kv.getTimestamp() + " " );     
            System.out.println(new String(kv.getValue()));     
	    //    int value = ByteBuffer.wrap(kv.getValue()).getInt();   
	}
	table.close();
    }     
    
    public static Result getOneRecordDoubles (String tableName, String rowKey, String family) throws IOException{     
        HTable table = new HTable(conf, tableName);     
        Get get = new Get(rowKey.getBytes());     
        get.addFamily(family.getBytes());
        return table.get(get);     
	
        /*
	  System.out.println("######## PRINTING LIST OF COORDINATES (HBASE TABLE V4 VERSION) ########");
	  
	  for(KeyValue kv : rs.raw()){     
          //  System.out.print(new String(kv.getRow()) + " " );     
	  //   System.out.print(new String(kv.getFamily()) + ":" ); 
	  System.out.print(new String(kv.getQualifier(),0,8));
	  System.out.print(ByteBuffer.wrap(kv.getQualifier(),8,8).getLong() + " = " );     
	  //   System.out.print(kv.getTimestamp() + " " );     
	  System.out.println("("+ ByteBuffer.wrap(kv.getValue(),0,8).getDouble() + "," + ByteBuffer.wrap(kv.getValue(),8,8).getDouble()+"," + ByteBuffer.wrap(kv.getValue(),16,8).getDouble()+")");     
	  //    int value = ByteBuffer.wrap(kv.getValue()).getInt();
	  
	  }
	  table.close();
	*/
    }
    
    public static void getAllRecord (String tableName) {     
        try{     
	    HTable table = new HTable(conf, tableName);     
	    Scan s = new Scan();     
	    ResultScanner ss = table.getScanner(s);     
	    for(Result r:ss){     
		for(KeyValue kv : r.raw()){     
		    
                    //System.out.print(new String(kv.getRow()) + " ");     
                    //System.out.print(new String(kv.getFamily()) + ":");     
                    //System.out.print(new String(kv.getQualifier()) + " = ");     
		    // System.out.print(kv.getTimestamp() + " ");     
                    System.out.println(new String(kv.getValue()));     
		}     
	    }
	    table.close();
        } catch (IOException e){     
            e.printStackTrace();     
        }     
    }
}
