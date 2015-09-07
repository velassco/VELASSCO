import java.io.IOException;
import java.util.List;
import java.util.StringTokenizer;

import org.apache.hadoop.hbase.Cell;
import org.apache.hadoop.hbase.CellUtil;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.io.ImmutableBytesWritable;
import org.apache.hadoop.hbase.mapreduce.TableMapper;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper.Context;



public class Mapper extends TableMapper<Text, IntWritable> {
	
	 private Text word = new Text();
	 private IntWritable one = new IntWritable();

	  public void map(ImmutableBytesWritable row, Result value, Context context) throws InterruptedException, IOException {
		  one.set(1);	
		  StringTokenizer numbers_List = new StringTokenizer(value.toString());
			
		  List<Cell> cells = value.listCells();
		  for (Cell cell : cells){
			  //System.out.println(new String(cell.getValue()));
			  numbers_List = new StringTokenizer(new String(cell.getValue()));
		  }
			  
		  while (numbers_List.hasMoreTokens()) {
			  word.set(numbers_List.nextToken());
		      context.write(word, one);
		  }
	  }
}


