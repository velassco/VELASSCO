package velassco.datainjection.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.OutputStreamWriter;

import net.sf.json.JSON;
import net.sf.json.xml.XMLSerializer;

import org.apache.commons.codec.binary.Base64;
import org.codehaus.jettison.json.JSONObject;
import org.json.JSONException;
import org.json.XML;

import velassco.datainjection.bean.Response;
import velassco.datainjection.bean.ResponseXML;
import velassco.datainjection.bean.Response.Cell;
import velassco.datainjection.bean.ResponseXML.CellXML;
import velassco.datainjection.bean.ResponseXML.RowXML;

import com.google.gson.Gson;
import com.google.gson.JsonElement;
import com.google.gson.JsonParser;

public class Json2Map {

	/**
	 * @param args
	 * @throws FileNotFoundException 
	 * @throws JSONException 
	 * @throws org.codehaus.jettison.json.JSONException 
	 */
	public static void main(String[] args) throws FileNotFoundException, JSONException, org.codehaus.jettison.json.JSONException {
		// TODO Auto-generated method stub   
		BufferedReader br = new BufferedReader(new FileReader("C:/Users/A514364/Desktop/response_all.json"));  
				     
		//convert the json string back to object  
		Gson gson = new Gson();  
		Response responseObj = gson.fromJson(br, Response.class); 
		System.out.println(responseObj);	

		ResponseXML responseXML = new ResponseXML(responseObj.rows.length);
		//decoding cells
		for (int r=0; r<responseObj.rows.length; r++){
			System.out.println("r = " + r);
			// decoding key
			String keyUndecoded = responseObj.rows[r].getKey();
			byte[] decodedData = Base64.decodeBase64(keyUndecoded.getBytes());
			String strDecoded = new String(decodedData);
			responseObj.rows[r].setKey(strDecoded);
			RowXML rowXML = new RowXML(responseObj.rows[r].getCells().length);
			responseXML.rows[r] = rowXML;
			rowXML.setKey(strDecoded);

			// decoding cells fields
			int index = 0;
			for (Cell c : responseObj.rows[r].getCells()){
				// decoding column
				String column = c.getColumn();
				byte[] columnDecoded = Base64.decodeBase64(column.getBytes());
				String columnStrDecoded = new String(columnDecoded);
				c.setColumn(columnStrDecoded);
				
				// decoding timestamp
				float timestamp = c.getTimestamp();
//				byte[] timestampDecoded = Base64.decodeBase64(Float.toString(timestamp).getBytes());
//				String timestampStrDecoded = new String(timestampDecoded);
//				c.setColumn(timestampStrDecoded);
				
				// decoding $
				String $ = c.get$();
				byte[] $Decoded = Base64.decodeBase64($.getBytes());
				String $StrDecoded = new String($Decoded);
				c.set$($StrDecoded);
				
				// adding c
				CellXML xmlCell = new CellXML();
				xmlCell.setValue($StrDecoded);
				xmlCell.setColumn(columnStrDecoded);
				xmlCell.setTimestamp(timestamp);
				rowXML.setCell(index,xmlCell);
				index++;
			}	
		}
		
		
		System.out.println();
		System.out.println("Decoded: " + responseObj);
		
		String jsonString = gson.toJson(responseObj);
		System.out.println("jsonString: " + jsonString);
		
		String xmlString = XML.toString(responseObj);
		System.out.println("xmlString: " + xmlString);
		
//		JsonElement json = new JsonParser().parse(jsonString);
//		XMLSerializer xmlSerializer = new XMLSerializer();
//		String xmlString2 = xmlSerializer.write(gson.to);
		JSONObject jsonObject2 = new JSONObject(jsonString);
	    String xmlString2 = XML.toString(jsonObject2);
		System.out.println("xmlString2: " + xmlString2);
		
//		File myFile = new File("/sdcard/myjsonstuff.txt");
//        myFile.createNewFile();
//        FileOutputStream fOut = new FileOutputStream(myFile);
//        OutputStreamWriter myOutWriter =new OutputStreamWriter(fOut);
//        myOutWriter.append(responseObj);
//        myOutWriter.close();
//        fOut.close();
		
		System.out.println("ResponseXML" + responseXML.toString());
	}

}
