/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package GetBoundaryOfAMesh;

import java.io.BufferedReader;
import org.apache.flink.addons.hbase.TableInputFormat;
import org.apache.flink.api.common.functions.FlatMapFunction;
import org.apache.flink.api.common.functions.GroupReduceFunction;

import org.apache.flink.api.java.operators.DataSink;
import org.apache.flink.api.java.ExecutionEnvironment;
import org.apache.flink.api.java.tuple.Tuple2;
import org.apache.flink.util.Collector;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.client.Scan;
import org.apache.hadoop.hbase.util.Bytes;

import org.apache.hadoop.hbase.Cell;
import org.apache.hadoop.hbase.CellUtil;

import org.apache.hadoop.fs.Path;

import java.nio.charset.Charset;

import org.apache.hadoop.hbase.filter.Filter;
import org.apache.hadoop.hbase.filter.FilterList;
import org.apache.hadoop.hbase.filter.ColumnPrefixFilter;

import java.util.Iterator;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;

/**
 *
 * @author icores
 */
public class GetBoundaryOfAMesh {
    

    
    public static void println(String s) {
        System.out.println(s);
    }

    public static void print(String s) {
        System.out.print(s);
    }

    final protected static char[] hexArray = "0123456789abcdef".toCharArray();

    //This function is a copy-paste from the YARN code.
    public static String toHexString(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for (int i = 0; i < bytes.length; i++) {
            int v = bytes[i] & 0xff;
            hexChars[i * 2] = hexArray[v >> 4];
            hexChars[i * 2 + 1] = hexArray[v & 0x0f];
        }
        return new String(hexChars);
    }

    //This function is a copy-paste from the YARN code.
    public static byte[] fromHexString(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) (((Character.digit(s.charAt(i), 16) << 4)
                    | Character.digit(s.charAt(i + 1), 16)) & 0xff);
        }
        return data;
    }
    
    //This function is a copy-paste from the YARN code.
    public static byte[] properModelID(String table_name, String modelID_hexaStr) {
        return modelID_hexaStr.getBytes();
    }

    //This function is a copy-paste from the YARN code.
    public static String getPrintableModelID(byte[] modelID) {
        String ret_value = new String(modelID);
        if (modelID.length == 16) {
            ret_value = toHexString(modelID);
        } else { // it's already a 32 hexadecimal string
            ret_value = new String(modelID);
        }
        return ret_value;
    }    
    
    
    //This function is a copy-paste from the YARN code.
    private static void deleteFolderIfExists(Path path) {
        try {
            Configuration conf = new Configuration();
            FileSystem fs = FileSystem.get(conf);
            boolean ok = fs.delete(path, true); // true for recursion
        } catch (Exception e) {
            println("Error deleting " + path.getName() + ": " + e.getMessage());
        }
    }
    
    //This function is a copy-paste from the YARN code.
    public static ByteBuffer getCompleteStartRowkey(String table_name, byte[] modelID,
            String meshType,
            String analysisName_in, double stepValue_in) { 
        String analysisName = "";
        double stepValue = 0.0;
        if ( meshType.equalsIgnoreCase("static")) {
        } else if ( meshType.equalsIgnoreCase("dynamic")) {
            analysisName = analysisName_in;
            stepValue = stepValue_in;
        } else {
            println( "Error: unsupported mesh type = " + meshType);
            return null;
        }
      
        ByteBuffer retRowkey;
        int row_key_length = modelID.length
                + 2 * Bytes.toBytes(analysisName.length()).length // should be 8 chars
                + analysisName.length()
                + 2 * Bytes.toBytes(stepValue).length // should be 16 chars
                ; 
        String str_modelID = new String(modelID);
        byte[] rowkey_bytes = Bytes.toBytes(str_modelID
                + toHexString(Bytes.toBytes(analysisName.length()))
                + analysisName
                + toHexString(Bytes.toBytes(stepValue))
        ); 
        
        retRowkey = ByteBuffer.allocate(rowkey_bytes.length);
        retRowkey.put(rowkey_bytes);
        return retRowkey;
    }

    //This function is a copy-paste from the YARN code.
    public static ByteBuffer getCompleteStopRowkey(String table_name, byte[] modelID,
            String meshType,
            String analysisName_in, double stepValue_in) { 
        String analysisName = "";
        double stepValue = 0.0;
        if ( meshType.equalsIgnoreCase("static")) {
        } else if ( meshType.equalsIgnoreCase("dynamic")) {
            analysisName = analysisName_in;
            stepValue = stepValue_in;
        } else {
            println( "Error: unsupported mesh type = " + meshType);
            return null;
        }
        ByteBuffer retRowkey;
        int row_key_length = modelID.length
                + 2 * Bytes.toBytes(analysisName.length()).length 
                + analysisName.length()
                + 2 * Bytes.toBytes(stepValue).length 
                ; 
        String str_modelID = new String(modelID);
        byte[] rowkey_bytes = Bytes.toBytes(str_modelID
                + toHexString(Bytes.toBytes(analysisName.length()))
                + analysisName
                + toHexString(Bytes.toBytes(stepValue))
        ); 
        
        int last_idx = rowkey_bytes.length - 1;
        rowkey_bytes[last_idx]++; 
        
        retRowkey = ByteBuffer.allocate(rowkey_bytes.length);
        retRowkey.put(rowkey_bytes);
        return retRowkey;
    } 
    
    //Only for print results
    //This function is a copy-paste from the YARN code.
    private static List< String> getReducerHexadecimalOutput(Path path) {
        List< String> ret_lst = null;
        try {
            Configuration conf = new Configuration();
            FileSystem fs = FileSystem.get(conf);
            InputStreamReader fi = new InputStreamReader(fs.open(path));
            BufferedReader br = new BufferedReader(fi);
            String line = br.readLine();
            if (line != null)
            {
                ret_lst = new ArrayList< String>();
            }
            while (line != null) {
                MR_BoundaryTriangle tri = new MR_BoundaryTriangle();
                if ( tri.fromString(line)) {
                    ret_lst.add(line);
                } else {
                    println( "Error reading triangle from line: " + line);
                    break;
                }
                line = br.readLine();
            }
            fi.close();
        } catch (Exception e) {
            println("Error getReducerTextOutput " + path.getName() + ": " + e.getMessage());
        }
        return ret_lst;
    }
    

    public static void main(String[] args) throws Exception {

        boolean print_output = false;

        final String sessionID = "0123456789abcdef";
        final String tableName = "Simulations_Data";
        final byte[] modelID = properModelID(tableName, "89d824150de3adc9198408d7bccb5f21");
        final int meshNumber = 1; //"Part";
        final String elementType = "Tetrahedra";
        final String meshType = "static";
        final String analysisName = "";
        final double stepValue = 0.0;
        
        println("\n>>> Doing GetBoundary of mesh " + meshNumber + " of model " + getPrintableModelID(modelID) + " from table " + tableName);
        println( "\t it is a " + meshType + " mesh with " + elementType);
        if ( meshType.equalsIgnoreCase("dynamic")) {
            println( "\t for analysys " + analysisName + " and step value = " + stepValue);
        }       
        
        boolean error = false;
        String output_str = "Configuring MR job.";
        String output_folder = "boundary_mesh_" + sessionID + "_" + getPrintableModelID(modelID);
        Path output_path = new Path(output_folder.toLowerCase());
        deleteFolderIfExists(output_path);

        println(output_path.toString());
             
        
        final String qualifierPrefixFormat = "m%06dcn_";
        
        
        final ExecutionEnvironment env = ExecutionEnvironment.getExecutionEnvironment();

        @SuppressWarnings("serial")
        //The TableInputFormat has to override three functions: getTableName, getScanner and mapResultToTuple
        DataSink<MR_BoundaryTriangle > hbaseDs = env.createInput(new TableInputFormat<Tuple2<List<MR_BoundaryTriangle>, Integer>>() {

            
            //This function is a copy-paste from the YARN code.
            long[] getConnectivityFromValue( Cell value) {
                long[] nodes = new long[ 4];
                boolean debug = false;
                if ( debug) {
                    byte[] qual = CellUtil.cloneQualifier( value);
                    String qual_str = new String( qual);
                    // may be we should ensure that the qualifier is ours... 'm000001...'
                    if ( qual_str.charAt(0) != 'm') {
                        println( "ERROR getConnectivityFromValue: wrong qualifier: " + qual_str);
                        return nodes;
                    }
                }

                byte[] raw_lst_nodes = CellUtil.cloneValue( value);
                if ( debug) {
                    if ( raw_lst_nodes.length != 4 * 8) { // tetrahedron = 4 node id's  and node id = int64
                        println( "ERROR getConnectivityFromValue: wrong value length 32 != " + raw_lst_nodes.length);
                        return nodes;
                    }
                }
                
                nodes[ 0] = Bytes.toLong( raw_lst_nodes);
                nodes[ 1] = Bytes.toLong( raw_lst_nodes, 8);
                nodes[ 2] = Bytes.toLong( raw_lst_nodes, 16);
                nodes[ 3] = Bytes.toLong( raw_lst_nodes, 24);

                return nodes;
            }
            
            //This function is a copy-paste from the YARN code.
            void addTetrahedronFacesToHash( long[] tetrahedronNodes) {
                MR_BoundaryTriangle tri;
                Integer prev;
                tri = new MR_BoundaryTriangle(tetrahedronNodes[ 0], tetrahedronNodes[ 1], tetrahedronNodes[ 2]);
                prev = _triangles.put(tri, 1);
                if ( prev != null) _triangles.put(tri, prev + 1); // if triangle already present, increase coutning
                tri = new MR_BoundaryTriangle(tetrahedronNodes[ 1], tetrahedronNodes[ 0], tetrahedronNodes[ 3]);
                prev = _triangles.put(tri, 1);
                if ( prev != null) _triangles.put(tri, prev + 1); // if triangle already present, increase coutning
                tri = new MR_BoundaryTriangle(tetrahedronNodes[ 2], tetrahedronNodes[ 1], tetrahedronNodes[ 3]);
                prev = _triangles.put(tri, 1);
                if ( prev != null) _triangles.put(tri, prev + 1); // if triangle already present, increase coutning
                tri = new MR_BoundaryTriangle(tetrahedronNodes[ 3], tetrahedronNodes[ 0], tetrahedronNodes[ 2]);
                prev = _triangles.put(tri, 1);
                if ( prev != null) _triangles.put(tri, prev + 1); // if triangle already present, increase coutning
            }

    
    
            @Override
            public String getTableName() {
                return tableName;
            }

            //This scanner is a copy-paste from the YARN code.
            @Override
            protected Scan getScanner() {

                ByteBuffer rowkey_mesh = getCompleteStartRowkey(tableName, modelID,
                        meshType, analysisName, stepValue); // partitionID) 
                ByteBuffer stop_rowkey_mesh = getCompleteStopRowkey(tableName, modelID,
                        meshType, analysisName, stepValue); // partitionID) 
                if ((rowkey_mesh == null) || (stop_rowkey_mesh == null)) {
                    return null;
                }

                String va = new String(rowkey_mesh.array(), Charset.forName("UTF-8"));
                String vb = new String(stop_rowkey_mesh.array(), Charset.forName("UTF-8"));
                //println(" iCORES: start rowkey: " + va.toString() + "   stop_rowkey: " + vb.toString());

                Scan scan_mesh_elements = new Scan();
                // check for M:c*
                // from the scan get Column Family "M:c" but not "M:m*"
                //get_model_id.addColumn( Bytes.toBytes( "M"), Bytes.toBytes( "m"));
                // scan only the Column Family = "M"
                scan_mesh_elements.addFamily(Bytes.toBytes("M"));

                // get only rows with this prefix:
                // Filter row_filter = new PrefixFilter(rowkey_dynamic_mesh.array());
                // faster to use StartRow and StopRow than row_filter. row_filter scans alls rows !!!
                scan_mesh_elements.setStartRow(rowkey_mesh.array());
                scan_mesh_elements.setStopRow(stop_rowkey_mesh.array());
                // from the qualifiers, get only qualifiers with prefix "x" (both solutions work)
                // look for the mesh conectivity
                String qualifierPrefix = String.format(qualifierPrefixFormat, meshNumber);
                Filter col_filter = new ColumnPrefixFilter(Bytes.toBytes(qualifierPrefix));
                //Filter col_filter = new QualifierFilter( CompareFilter.CompareOp.EQUAL, new BinaryPrefixComparator( Bytes.toBytes( "m")));
                // combine both filters: row and qualifier prefixes:
                FilterList filter_list = new FilterList(FilterList.Operator.MUST_PASS_ALL);
                // filter_list.addFilter(row_filter);
                filter_list.addFilter(col_filter);
                // add filter to the scan
                scan_mesh_elements.setFilter(filter_list);

                return scan_mesh_elements;
            }

            //This integer (second part of the tuple) is not needed, but the TableInputFormat needs a tuple, so I add a useless integer.
            private Tuple2<List<MR_BoundaryTriangle>, Integer> outputList = new Tuple2<List<MR_BoundaryTriangle>, Integer>();
            
            HashMap< MR_BoundaryTriangle, Integer> _triangles;

            //This function creates a tuple from the Result object provided by HBase
            //I USE THIS FUNCTION AS THE MAPPER, this code is a copy-paste from the YARN code.
            @Override
            protected Tuple2<List<MR_BoundaryTriangle>, Integer> mapResultToTuple(Result value) {

                outputList.f0 = new ArrayList<MR_BoundaryTriangle>();
                boolean write_output = false;

                if (value.advance()) {
                    _triangles = new HashMap< MR_BoundaryTriangle, Integer>(64 * 1024 * 1024, 0.9f); // initialCapacity, loadFactor

                    long[] tetrahedron = null;
                    tetrahedron = getConnectivityFromValue(value.current());
                   
                    addTetrahedronFacesToHash(tetrahedron);
                    write_output = true;

                    while (value.advance()) {
                        tetrahedron = getConnectivityFromValue(value.current());
                        addTetrahedronFacesToHash(tetrahedron);
                    }
                }
                

                if (write_output) {

                    if (_triangles != null) {
                        for (Map.Entry pair : _triangles.entrySet()) {
                            Integer val = (Integer) (pair.getValue());
                            if (val.equals(1)) {
                                Object k = pair.getKey();
                                String txt = "class " + k.getClass();
                                MR_BoundaryTriangle tri = (MR_BoundaryTriangle) k;

                                outputList.f0.add(tri);
                                outputList.f1 = 1;
                            } else {
                                // do not output repeated triangs
                            }
                        }
                    }
                }

                return outputList;
                
            }

            
        }).flatMap( new ListSplitter() ).groupBy(0).reduceGroup(new MyReducer() ).writeAsText(output_path.toString()).setParallelism(1);
        // setParallelism() is used to create only 1 single output file.
        // The flatMap function (ListSplitter) only transform the List of triangles to the collector that is required by the reducer.
        // I think the mapper code could be integrated into the ListSplitter function, but I think is more easy to understand the code is in two diferent functions.
        
        
        //hbaseDs.print(); //This only works if using DataSet instead of DataSink
        
        // kick off execution.
        env.execute();        
        
        
        //This is for check the number of triangles.
        List< String> lines = getReducerHexadecimalOutput(output_path);
        int num_elements = 0;
        if ((lines != null) && (lines.size() > 0)) {
            output_str = "";
            for (String ln : lines) {
                if (print_output) {
                    output_str += ln + "\n";
                }
                num_elements++;
            }
            output_str += "Number of triangles = " + num_elements + "\n";
        } else {
            output_str = "Error: model data not found for modelID " + getPrintableModelID(modelID) + " from table " + tableName;
            error = true;
        }
        System.out.println(output_str);
    
    }

    
    
    // The flatMap function (ListSplitter) only transform the List of triangles to the collector that is required by the reducer.
    public static class ListSplitter implements FlatMapFunction< Tuple2<List<MR_BoundaryTriangle>, Integer>, Tuple2<MR_BoundaryTriangle, Integer>> {
        @Override
        public void flatMap(Tuple2<List<MR_BoundaryTriangle>, Integer> line, Collector<Tuple2<MR_BoundaryTriangle, Integer>> out) {

            if (!line.f0.isEmpty())
            {
                    Tuple2<MR_BoundaryTriangle, Integer> record = new Tuple2<MR_BoundaryTriangle, Integer>();
                    
                    Iterator<MR_BoundaryTriangle> iter = line.f0.iterator();
                    while (iter.hasNext()) {
                        record.setField(iter.next(), 0);
                        record.setField(1, 1);
                        out.collect(record);
                    }
            }
            
        }
    }   
    
    
    public static class MyReducer implements GroupReduceFunction< Tuple2<MR_BoundaryTriangle, Integer>, MR_BoundaryTriangle> {
    
        @Override
        public void reduce( Iterable< Tuple2<MR_BoundaryTriangle, Integer>> values, Collector<MR_BoundaryTriangle> out) {
            
            MR_BoundaryTriangle tri = null;
            
            int count = 0;
            for (Tuple2<MR_BoundaryTriangle, Integer> i : values) {
             count++;
             tri = i.f0;
             if (count > 1)
             {
                 break;
             }
            } 
             
            if (count <= 1) 
            {
                if (tri != null)
                {
                        out.collect(tri);
                }
            }
  
             
        }
    }          

    
    
}

