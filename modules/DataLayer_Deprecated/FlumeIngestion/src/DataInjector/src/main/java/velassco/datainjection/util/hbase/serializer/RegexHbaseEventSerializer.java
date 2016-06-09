package velassco.datainjection.util.hbase.serializer;

/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.codec.binary.Hex;
import org.apache.commons.lang.RandomStringUtils;
import org.apache.commons.lang.StringEscapeUtils;
import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.FlumeException;
import org.apache.flume.conf.ComponentConfiguration;
import org.apache.hadoop.hbase.KeyValue;
import org.apache.hadoop.hbase.client.Increment;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.Row;
import org.apache.hadoop.hbase.util.Bytes;
import org.apache.flume.sink.hbase.HbaseEventSerializer;

import com.google.common.collect.Lists;

import java.nio.charset.StandardCharsets;

/**
 * An {@link HbaseEventSerializer} which parses columns based on a supplied
 * regular expression and column name list.
 * 
 * Note that if the regular expression does not return the correct number of
 * groups for a particular event, or it does not correctly match an event, the
 * event is silently dropped.
 * 
 * Row keys for each event consist of a timestamp concatenated with an
 * identifier which enforces uniqueness of keys across flume agents.
 * 
 * See static constant variables for configuration options.
 */
public class RegexHbaseEventSerializer implements HbaseEventSerializer {
	// Config vars
	/** Regular expression used to parse groups from event data. */
	public static final String REGEX_CONFIG = "regex";
	public static final String REGEX_DEFAULT = "(.*)";
	/** Whether to ignore case when performing regex matches. */
	public static final String IGNORE_CASE_CONFIG = "regexIgnoreCase";
	public static final boolean INGORE_CASE_DEFAULT = false;
	/** Comma separated list of column names to place matching groups in. */
	public static final String COL_NAME_CONFIG = "colNames";
	public static final String COLUMN_NAME_DEFAULT = "payload";
	/** Index of the row key in matched regex groups */
	public static final String ROW_KEY_INDEX_CONFIG = "rowKeyIndex";
	/** Placeholder in colNames for row key */
	public static final String ROW_KEY_NAME = "ROW_KEY";
	/** Whether to deposit event headers into corresponding column qualifiers */
	public static final String DEPOSIT_HEADERS_CONFIG = "depositHeaders";
	public static final boolean DEPOSIT_HEADERS_DEFAULT = false;
	/** What charset to use when serializing into HBase's byte arrays */
	public static final String CHARSET_CONFIG = "charset";
	public static final String CHARSET_DEFAULT = "UTF-8";
	/*
	 * This is a nonce used in HBase row-keys, such that the same row-key never
	 * gets written more than once from within this JVM.
	 */
	protected static final AtomicInteger nonce = new AtomicInteger(0);
	protected static String randomKey = RandomStringUtils
			.randomAlphanumeric(10);
	protected byte[] cf;
	private byte[] payload;
	private List<byte[]> colNames = Lists.newArrayList();
	private Map<String, String> headers;
	private boolean regexIgnoreCase;
	private boolean depositHeaders;
	private Pattern inputPattern;
	private Charset charset;
	private int rowKeyIndex;

	public void configure(Context context) {
		String regex = context.getString(REGEX_CONFIG, REGEX_DEFAULT);
		regexIgnoreCase = context.getBoolean(IGNORE_CASE_CONFIG,
				INGORE_CASE_DEFAULT);
		depositHeaders = context.getBoolean(DEPOSIT_HEADERS_CONFIG,
				DEPOSIT_HEADERS_DEFAULT);
		inputPattern = Pattern.compile(regex, Pattern.DOTALL
				+ (regexIgnoreCase ? Pattern.CASE_INSENSITIVE : 0));
		charset = Charset.forName(context.getString(CHARSET_CONFIG,
				CHARSET_DEFAULT));
		String colNameStr = context.getString(COL_NAME_CONFIG,
				COLUMN_NAME_DEFAULT);
		String[] columnNames = colNameStr.split(",");
		for (String s : columnNames) {
			colNames.add(s.getBytes(charset));
		}
		// Rowkey is optional, default is -1
		rowKeyIndex = context.getInteger(ROW_KEY_INDEX_CONFIG, -1);
		// if row key is being used, make sure it is specified correct
		if (rowKeyIndex >= 0) {
			if (rowKeyIndex >= columnNames.length) {
				throw new IllegalArgumentException(ROW_KEY_INDEX_CONFIG
						+ " must be " + "less than num columns "
						+ columnNames.length);
			}
			if (!ROW_KEY_NAME.equalsIgnoreCase(columnNames[rowKeyIndex])) {
				throw new IllegalArgumentException("Column at " + rowKeyIndex
						+ " must be " + ROW_KEY_NAME + " and is "
						+ columnNames[rowKeyIndex]);
			}
		}
	}

	public void configure(ComponentConfiguration conf) {
	}


	public void initialize(Event event, byte[] columnFamily) {
		this.headers = event.getHeaders();
		this.payload = event.getBody();
		this.cf = columnFamily;
	}

	/**
	 * Returns a row-key with the following format: [time in millis]-[random
	 * key]-[nonce]
	 */
	protected byte[] getRowKey(Calendar cal) {
		/*
		 * NOTE: This key generation strategy has the following properties:
		 * 
		 * 1) Within a single JVM, the same row key will never be duplicated. 2)
		 * Amongst any two JVM's operating at different time periods (according
		 * to their respective clocks), the same row key will never be
		 * duplicated. 3) Amongst any two JVM's operating concurrently
		 * (according to their respective clocks), the odds of duplicating a
		 * row-key are non-zero but infinitesimal. This would require
		 * simultaneous collision in (a) the timestamp (b) the respective nonce
		 * and (c) the random string. The string is necessary since (a) and (b)
		 * could collide if a fleet of Flume agents are restarted in tandem.
		 * 
		 * Row-key uniqueness is important because conflicting row-keys will
		 * cause data loss.
		 */
		String rowKey = String.format("%s-%s-%s", cal.getTimeInMillis(),
				randomKey, nonce.getAndIncrement());
		return rowKey.getBytes(charset);
	}

	protected byte[] getRowKey() {
		return getRowKey(Calendar.getInstance());
	}

	// **** OSLO MEETING: using md5( FullPath, SimulationName, UserName) = 16 bytes as SimulationID
	// return MD5 hash for ROWKEY input
    public byte[] getMD5(String input) {
        MessageDigest msg;
        String md5_32 = "";
        byte[] md5_16 = null; 
		try {
			msg = MessageDigest.getInstance("MD5");
	        msg.update(input.getBytes(), 0, input.length());
	        md5_32 = new BigInteger(1, msg.digest()).toString(16);
	        System.out.println("md5_32: " + md5_32);
	        md5_16 = hexStringToByteArray(md5_32);
	        System.out.println("md5_16: " + md5_16);
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return md5_16;
    }	

    // generate MD5 16bytes
    public static byte[] hexStringToByteArray(String s) {
	    int len = s.length();
	    byte[] data = new byte[len / 2];
	    for (int i = 0; i < len; i += 2) {
	        data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
	                             + Character.digit(s.charAt(i+1), 16));
	    }
	    return data;
	}
    
    public byte[] getArrayRowkey(String simulId, String analysisName, String step, String partitionId) {
    	// GIUSSEPE'S CODE
    	int length_of_String = Bytes.toBytes(analysisName).length;
    	byte[] simulIdMD5_a6 = hexStringToByteArray(simulId);    	
    	//int rowkey_length = simulId.length()+4+length_of_String+8;
    	int rowkey_length = simulIdMD5_a6.length+4+length_of_String+8;
    	if (partitionId!=null)
    		rowkey_length += 4;
    	System.out.println("rowkey_length: " + rowkey_length);
    	ByteBuffer rowkey = ByteBuffer.allocate(rowkey_length);
    	rowkey.clear();
    	//rowkey.put(Bytes.toBytes(simulId));
    	rowkey.put(simulIdMD5_a6);
    	rowkey.putInt(length_of_String);
    	rowkey.put(Bytes.toBytes(analysisName));
    	rowkey.putDouble(Double.parseDouble(step));
    	if (partitionId!=null)  
    		rowkey.putInt(Integer.parseInt(partitionId));
    	return rowkey.array();
    }	
    

	public List<Row> getActions() throws FlumeException {
		List<Row> actions = Lists.newArrayList();
		byte[] rowKey;
		String rowKeyStr = "";
		ByteBuffer rowKeyBB ;
		Matcher m = inputPattern.matcher(new String(payload, charset));
		if (!m.matches()) {
			return Lists.newArrayList();
		}
		if (m.groupCount() != colNames.size()) {
			return Lists.newArrayList();
		}
		
		try
        {            
           if(rowKeyIndex < 0)
                rowKey = getRowKey();
            else {
                rowKeyStr = m.group(rowKeyIndex + 1);
                System.out.println("\n\nrowKeyStr: " + rowKeyStr);
                if (rowKeyStr.contains("+")) {
	                String rowKeyFields[] = rowKeyStr.split("\\+"); // SIMUL_ID + "+" + analysisName.length() + "+" + analysisName + "+" + timestep + "+" + partitionId
	                String simulID = rowKeyFields[0];
	                String analysisName = rowKeyFields[1];
	                String timestep = rowKeyFields[2];
	                String partId = (rowKeyFields.length == 4 ? rowKeyFields[3] : null);
	                //boolean isKeyData = ( !partId.equals("") ? true : false );
	                rowKey = getArrayRowkey(simulID, analysisName, timestep, partId);
                } else {
                	byte[] simulIdMD5_a6 = hexStringToByteArray(rowKeyStr);
                	ByteBuffer rowkey = ByteBuffer.allocate(16);
                	rowkey.clear();
                	rowkey.put(simulIdMD5_a6);
                	rowKey = rowkey.array();
                }
	//            	System.out.println("** ROWKEY_SIZE: " + rowKey.length);
	//                rowKeyStr = new String(rowKey, StandardCharsets.UTF_8); ;
	//                System.out.println("** ROWKEY_STRING: " + rowKeyStr + " IN " + new String(cf));
	//                System.out.println("** ROWKEY_STRING_bytes: " + rowKeyStr.getBytes().length);
	//        		ByteBuffer rowkeyBB = ByteBuffer.allocate(rowKey.length);
	//        		rowkeyBB.clear();
	//        		rowkeyBB.put(Bytes.toBytes((rowKeyStr)));
	//        		rowKey = rowkeyBB.array();
            }     
           Put put = new Put(rowKey);
       	   Pattern qualifierPattern = Pattern.compile("(.+?)\\|\\|", Pattern.DOTALL + (regexIgnoreCase ? Pattern.CASE_INSENSITIVE : 0));
       	   String regExp = m.group(rowKeyIndex + 2);
       	   System.out.println("regExp: " + regExp );
       	   Matcher qualifierMatcher = qualifierPattern.matcher(regExp);
       	   while (qualifierMatcher.find()){
       		   String cData = qualifierMatcher.group(1); 
    		   String cValue[] = cData.split(":");
    		   String qualifier = cValue[0];
    		   String type_value[] = cValue[1].split("=");
    		   String type = type_value[0];
    		   //String value = ( !type_value[1].equals(null) ? type_value[1] : "0");
    		   String value = type_value[1];
    		   if (type.equals("1integer")){
    			   int intValue = 0;
    			   if (value!=null && !value.trim().isEmpty()) {
    				   intValue = Integer.parseInt(value);
    			   }	       			
    	           //ByteBuffer integerBB = ByteBuffer.allocate(4); // 4 Bytes
    	           //integerBB.put(Bytes.toBytes(Integer.parseInt(value)));     	           
    			   //put.add(cf, Bytes.toBytes(qualifier), integerBB.array());
    			   put.add(cf, Bytes.toBytes(qualifier), Bytes.toBytes(intValue));
    	           //integerBB.clear();
    		   } else if (type.equals("2integer")){
    			   int[] intArray = {0,0};
    			   if (value!=null && !value.trim().isEmpty()) {
    				   String[] intValues = value.split(" ");
    				   intArray = new int[] {Integer.parseInt(intValues[0]), Integer.parseInt(intValues[1])};
    			   }
    			   
    	           ByteBuffer integerVectorBB = ByteBuffer.allocate(8); // 8 Bytes
    	           integerVectorBB.putInt(intArray[0]);
    	           integerVectorBB.putInt(intArray[1]);
    	           //integerVectorBB.put(Bytes.toBytes(Integer.parseInt(intValues[1])));
    	           //integerVectorBB.put(Bytes.toBytes(Integer.parseInt(intValues[2])));
    	           put.add(cf, Bytes.toBytes(qualifier), integerVectorBB.array());
    	           //integerVectorBB.clear();
    		   } else if (type.equals("3integer")){
    			   int[] intArray = {0,0,0};
    			   if (value!=null && !value.trim().isEmpty()) {
    				   String[] intValues = value.split(" ");
    				   intArray = new int[] {Integer.parseInt(intValues[0]), Integer.parseInt(intValues[1]), Integer.parseInt(intValues[2])};
    			   }
    			   
    	           ByteBuffer integerVectorBB = ByteBuffer.allocate(12); // 12 Bytes
    	           integerVectorBB.putInt(intArray[0]);
    	           integerVectorBB.putInt(intArray[1]);
    	           integerVectorBB.putInt(intArray[2]);
    	           //integerVectorBB.put(Bytes.toBytes(Integer.parseInt(intValues[1])));
    	           //integerVectorBB.put(Bytes.toBytes(Integer.parseInt(intValues[2])));
    	           put.add(cf, Bytes.toBytes(qualifier), integerVectorBB.array());
    	           //integerVectorBB.clear();
    			       			   
    		   } else if (type.equals("4integer")){
    			   int[] intArray = {0,0,0, 0};
    			   if (value!=null && !value.trim().isEmpty()) {
    				   String[] intValues = value.split(" ");
    				   intArray = new int[] {Integer.parseInt(intValues[0]), Integer.parseInt(intValues[1]), Integer.parseInt(intValues[2]), Integer.parseInt(intValues[3])};
    			   }
    			   
    	           ByteBuffer integerVectorBB = ByteBuffer.allocate(16); // 16 Bytes
    	           integerVectorBB.putInt(intArray[0]);
    	           integerVectorBB.putInt(intArray[1]);
    	           integerVectorBB.putInt(intArray[2]);
    	           integerVectorBB.putInt(intArray[3]);
    	           //integerVectorBB.put(Bytes.toBytes(Integer.parseInt(intValues[1])));
    	           //integerVectorBB.put(Bytes.toBytes(Integer.parseInt(intValues[2])));
    	           put.add(cf, Bytes.toBytes(qualifier), integerVectorBB.array());
    	           //integerVectorBB.clear();
    			       			   
    		   } else if (type.equals("1double")){
    			   double doubleValue = 0.0;
    			   if (value!=null && !value.trim().isEmpty()) {
    				   doubleValue = Double.parseDouble(value);
    			   }
    			   
    			   //ByteBuffer doubleBB = ByteBuffer.allocate(8); // 8 Bytes
    	           //doubleBB.put(Bytes.toBytes(Double.parseDouble(value))); 
    	           put.add(cf, Bytes.toBytes(qualifier), Bytes.toBytes(doubleValue));
    	           //doubleBB.clear();
    		   } else if (type.equals("3double")){
    			   double[] doubleArray = {0.0,0.0,0.0};
    			   if (value!=null && !value.trim().isEmpty()) {
    				   String[] doubleValues = value.split(" ");
    				   doubleArray = new double[] {Double.parseDouble(doubleValues[0]), Double.parseDouble(doubleValues[1]), Double.parseDouble(doubleValues[2])};
    			   }
    			   
    			   
    			   //String[] doubleValues = value.split(" ");
    	           ByteBuffer doubleVectorBB = ByteBuffer.allocate(24); // 24 Bytes
    	           doubleVectorBB.putDouble(doubleArray[0]);
    	           doubleVectorBB.putDouble(doubleArray[1]);
    	           doubleVectorBB.putDouble(doubleArray[2]);
    	           put.add(cf, Bytes.toBytes(qualifier), doubleVectorBB.array());
    	           //doubleVectorBB.clear();
    		   } else if (type.equals("4double")){
    			   double[] doubleArray = {0.0,0.0,0.0,0.0};
    			   if (value!=null && !value.trim().isEmpty()) {
    				   String[] doubleValues = value.split(" ");
    				   doubleArray = new double[] {Double.parseDouble(doubleValues[0]), Double.parseDouble(doubleValues[1]), Double.parseDouble(doubleValues[2]), Double.parseDouble(doubleValues[3])};
    			   }
    			   
    			   //String[] doubleValues = value.split(" ");
    	           ByteBuffer doubleVectorBB = ByteBuffer.allocate(32); // 32 Bytes
    	           doubleVectorBB.putDouble(doubleArray[0]);
    	           doubleVectorBB.putDouble(doubleArray[1]);
    	           doubleVectorBB.putDouble(doubleArray[2]);
    	           doubleVectorBB.putDouble(doubleArray[3]);
    	           put.add(cf, Bytes.toBytes(qualifier), doubleVectorBB.array());
    	           //doubleVectorBB.clear();
    		   } else if (type.equals("string")){ 
    			   // type=string  
    	           ByteBuffer stringBB = ByteBuffer.allocate(value.length()); // value.length Bytes
    	           stringBB.put(Bytes.toBytes(value)); 
    	           put.add(cf, Bytes.toBytes(qualifier), stringBB.array());
    	           //stringBB.clear();
    		   }
    		   
    		   List<KeyValue> kvList = put.get(cf, Bytes.toBytes(qualifier));
			   Iterator<KeyValue> it = kvList.iterator();
			   while (it.hasNext()) {
				   KeyValue kv = it.next();   
				   System.out.println("********* " + qualifier + ": " + kv.getValue());
			   }
    		      
     		   	   
       	   }
           
       	   
           //Append append = new Append(rowKey);
           //ByteBuffer rowkeyBB = ByteBuffer.allocate(64);
           //System.out.println("**** ROWKEY BYTES LENGTH: " + rowKey.length);
           //rowkeyBB.put(rowKey); //Bytes.toBytes(rowKeyStr));
           //System.out.println("**** ROWKEY BUFFER ARRAY: " + rowkeyBB.array());
           
//           short ss = Short.parseShort(rowKeyStr, 2);
//           Byte BB = new Byte((byte)ss);
//           System.out.println("BB ARRAY: " + BB.array().toString());
           
           //byte[] hexStringRowkey = hexStringToByteArray(rowKeyStr);
           //byte[] binaryRowkey = Hex.encodeHexString(rowKeyStr.getBytes()).getBytes();
           // Put put = new Put(rowKey); //toBytes(rowKey));
   		   //System.out.println(" #### PUT: " + new String(cf) + "-" + rowKeyStr);
           //put.add(cf, Bytes.toBytes("qualifier"), Bytes.toBytes("value"));
           //rowkeyBB.clear();
           
           // testing regexp
//       	   Pattern qualifierPattern = Pattern.compile("(.+?)\\|\\|", Pattern.DOTALL + (regexIgnoreCase ? Pattern.CASE_INSENSITIVE : 0));
//           System.out.println("** Text before matching regexp:  " + m.group(rowKeyIndex + 2));
//           System.out.println("** Checking if regexp exists:  " + m.group(rowKeyIndex + 2).indexOf('|'));
//           System.out.println("** Checking if regexp exists in string bytes:  " + new String (m.group(rowKeyIndex + 2).getBytes(StandardCharsets.ISO_8859_1)).indexOf('|'));
//       	   Matcher qualifierMatcher = qualifierPattern.matcher(m.group(rowKeyIndex + 2));
//       	   while (qualifierMatcher.find()){
//       		   String cData = qualifierMatcher.group(1); //.getBytes(StandardCharsets.UTF_8);
//       		   //       		   System.out.println("** Checking in qualifier-value: " + cData + " points: " + cData.indexOf(":"));
//       		   String cValue[] = cData.split(":");
//       		   String qualifier = cValue[0];
//       		   String valueStr = new String(cValue[1].getBytes(), StandardCharsets.US_ASCII);
//       		   ByteBuffer byteBuffer = ByteBuffer.wrap(cValue[1].getBytes()).asReadOnlyBuffer();
//           	   CharBuffer charBuffer = StandardCharsets.UTF_8.decode(byteBuffer);
//           	   String text = charBuffer.toString();
//           	   System.out.println("****** CharBuffer: " + text);
//       		   System.out.println("********* valueStr: " + valueStr);
//       		   byte[] value = Bytes.toBytes(text);
//       		   ByteBuffer valueBB = ByteBuffer.allocate(value.length);
//       		   valueBB.put(value);
////       	   System.out.println(" #### PUT: " + new String(cf) + "-" + qualifier + "-" + value);
////       	   System.out.println(" #### PUT BYTES: " + rowKey + "-" + new String(cf) + "-" + qualifier + "-" + value);
//       		   put.add(cf, Bytes.toBytes(qualifier), valueBB.array());
//       		   rowKey = null;
//       		   value = null;
//       		   valueBB.clear();
//               List<KeyValue> kvalues = put.get(cf, qualifier.getBytes());
//               for (int i=0; i<kvalues.size(); i++) {
//            	   KeyValue kv = kvalues.get(i);
//            	   System.out.println(" #### KEY: " + kv.getKeyString() + " / VALUE: LONG=" + kv.getValueLength() + " + Value = " + new String(kv.getValue()));;
//               }
               
//       		   Map<String, Object> fingerprint = new HashMap<String, Object>();
//       		   fingerprint = put.getFingerprint();
//	       		Iterator it = fingerprint.entrySet().iterator();
//	       	    while (it.hasNext()) {
//	       	        Map.Entry pair = (Map.Entry)it.next();
//	       	        System.out.println("Key-Value: " + pair.getKey() + " = " + pair.getValue());
//	       	        it.remove(); // avoids a ConcurrentModificationException
//	       	    }
//
//	       		Map<byte[], List<KeyValue>> familyMap = new HashMap<byte[], List<KeyValue>>();
//	       		familyMap = put.getFamilyMap();
//		       	Iterator fm = familyMap.entrySet().iterator();
//		       	while (fm.hasNext()) {
//		       	        Map.Entry pair = (Map.Entry)fm.next();
//		       	        System.out.println("Key-Value: " + pair.getKey() + " = " + pair.getValue());
//		       	        it.remove(); // avoids a ConcurrentModificationException
//		       	}
//       	   } 
       	   
//           System.out.println("**** Input: " + rowKey + " in " + m);
//           String[] columnsData = m.group(rowKeyIndex + 2).split("|"); //split("delCol");
//            for(String cData: columnsData){                
//                String cValue[] = cData.split(":");
//                System.out.println(" #### PUT: " + new String(cf) + "-" + cValue[0] + "-" + cValue[1]);
//                //append.add(cf, cValue[0].getBytes(Charsets.UTF_8), cValue[1].getBytes(Charsets.UTF_8));
//                String qualifier = cValue[0];
//                String value = cValue[1];
//                System.out.println(" #### PUT BYTES: " + rowKey + "-" + new String(cf) + "-" + Bytes.toBytes(qualifier) + "-" + Bytes.toBytes(value));
//                put.add(cf, qualifier.getBytes(), value.getBytes());
////                float value = Float.parseFloat(cValue[1]); //.getBytes(StandardCharsets.UTF_8).toString();
////                ByteBuffer qualifierBB = ByteBuffer.allocate(64);
////                ByteBuffer valueBB = ByteBuffer.allocate(64);
////                qualifierBB.put(qualifier.getBytes());
////                valueBB.put(Bytes.toBytes(value));
////				put.add(cf, qualifierBB.array(),valueBB.array()); //value.getBytes(StandardCharsets.UTF_8));
////                qualifierBB.clear();
////                valueBB.clear();
//            }

            if (depositHeaders) {
				for (Map.Entry<String, String> entry : headers.entrySet()) {
					//append.add(cf, entry.getKey().getBytes(charset), entry.getValue().getBytes(charset));
					put.add(cf, entry.getKey().getBytes(charset), entry.getValue().getBytes(charset));
				}
			}
            //actions.add(append);
            System.out.println("********* ACTION: " + put.toJSON() );
 			   	
            actions.add(put);
            System.out.println("********* ACTION LIST: NUM = " + actions.size() + "\n");
        }
        catch(Exception e)
        {
            throw new FlumeException("Could not get row key!", e);
        }
        return actions;
	}


	public List<Increment> getIncrements() {
		return Lists.newArrayList();
	}


	public void close() {
	}
}

