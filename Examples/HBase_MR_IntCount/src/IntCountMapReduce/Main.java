import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.client.Scan;
import org.apache.hadoop.hbase.filter.ColumnRangeFilter;
import org.apache.hadoop.hbase.filter.Filter;
import org.apache.hadoop.hbase.filter.FilterList;
import org.apache.hadoop.hbase.mapreduce.TableMapReduceUtil;
import org.apache.hadoop.hbase.util.Bytes;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;


public class Main {
	public static void main(String[] args) throws IOException, ClassNotFoundException, InterruptedException {
		
		Configuration configuration = HBaseConfiguration.create();
	
		Job mr_job = Job.getInstance(configuration, "mr_job");
		mr_job.setJarByClass(Main.class);     
	
		TableMapReduceUtil.addDependencyJars(mr_job);
	
		Scan scan = new Scan();
	
		/*
		scan_job_DMV = new Scan(start_rowkey.array(),stop_rowkey.array());
		FilterList filterList_job_DMV = new FilterList(FilterList.Operator.MUST_PASS_ONE);
		Filter filter_for_coordinates_DMV = new ColumnRangeFilter(Bytes.toBytes("c000001_"), true,Bytes.toBytes("c000004_"), false);
		Filter filter_for_results_DMV = new ColumnRangeFilter(Bytes.toBytes("r000001_"), true,Bytes.toBytes("r000006_"), false);
		Filter filter_for_mesh_PCS = new ColumnRangeFilter(Bytes.toBytes("m000002_"), true,Bytes.toBytes("m000004_"), false);
		filterList_job_DMV.addFilter(filter_for_coordinates_DMV);
		filterList_job_DMV.addFilter(filter_for_results_DMV);
		filterList_job_DMV.addFilter(filter_for_mesh_PCS);
		scan_job_DMV.setFilter(filterList_job_DMV);
		 */	
   
		TableMapReduceUtil.initTableMapperJob("Integer_Numbers",		// input HBase table name
			   								scan,    		         	// Scan instance to control CF and attribute selection <key, value>
			   								Mapper.class,   			// mapper
			   								Text.class,       			// mapper output key
			   								IntWritable.class,			// mapper output value
			   								mr_job);
	       	
		TableMapReduceUtil.initTableReducerJob("MR_Results",			// output table
											Reducer.class, 				// reducer class
											mr_job);

		final long startTime = System.currentTimeMillis();
		boolean b = mr_job.waitForCompletion(true);
		
		if (!b) 
			throw new IOException("error with job!");
		
		final double duration = (System.currentTimeMillis() - startTime)/1000.0;
		System.out.println("Job Finished in " + duration + " seconds");
		System.exit(0);
	}
}