import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.io.ImmutableBytesWritable;
import org.apache.hadoop.hbase.mapreduce.TableReducer;
import org.apache.hadoop.hbase.util.Bytes;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer.Context;



public class Reducer extends TableReducer<Text, IntWritable, ImmutableBytesWritable> {

	//private IntWritable totalCount = new IntWritable();
	public void reduce(Text key, Iterable <IntWritable> values, Context context) throws IOException, InterruptedException {
		int count = 0;
		Iterator<IntWritable> it=values.iterator();
		
		while (it.hasNext()) 
		   count += it.next().get();
	
		System.out.println(key + " " + count);
		
		Put put = new Put(Bytes.toBytes(1));     
	    put.add(Bytes.toBytes("Occurrences"),Bytes.toBytes(key.toString()),Bytes.toBytes(String.valueOf(count)));     
	         		 
	    context.write(null, put);
		// totalCount.set(count);
		 /*
		  context.write(, count);
		*/
	}
}
