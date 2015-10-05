package velassco.datainjection.util.hbase.serializer;

import java.nio.charset.Charset;
import java.util.Calendar;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.apache.commons.lang.RandomStringUtils;
import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.FlumeException;
import org.apache.flume.conf.ComponentConfiguration;
import org.apache.hadoop.hbase.client.Increment;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.Row;



import com.google.common.base.Charsets;
import com.google.common.collect.Lists;

public class MultiCFRegexHbaseEventSerializer implements HbaseEventSerializer {
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
	protected static String randomKey = RandomStringUtils.randomAlphanumeric(10);
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
		regexIgnoreCase = context.getBoolean(IGNORE_CASE_CONFIG, INGORE_CASE_DEFAULT);
		depositHeaders = context.getBoolean(DEPOSIT_HEADERS_CONFIG, DEPOSIT_HEADERS_DEFAULT);
		inputPattern = Pattern.compile(regex, Pattern.DOTALL + (regexIgnoreCase ? Pattern.CASE_INSENSITIVE : 0));
		charset = Charset.forName(context.getString(CHARSET_CONFIG, CHARSET_DEFAULT));
		String colNameStr = context.getString(COL_NAME_CONFIG, COLUMN_NAME_DEFAULT);
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


	public List<Row> getActions() throws FlumeException {
		List<Row> actions = Lists.newArrayList();
		byte[] rowKey;
		Matcher m = inputPattern.matcher(new String(payload, charset));
		if (!m.matches()) {
			return Lists.newArrayList();
		}
		if (m.groupCount() != colNames.size()) {
			return Lists.newArrayList();
		}
		try {
			if (rowKeyIndex < 0) {
				rowKey = getRowKey();
			} else {
				rowKey = m.group(rowKeyIndex + 1).getBytes(Charsets.UTF_8);
			}
			Put put = new Put(rowKey);
			for (int i = 0; i < colNames.size(); i++) {
				if (i != rowKeyIndex) {
					put.add(cf, colNames.get(i),
							m.group(i + 1).getBytes(Charsets.UTF_8));
				}
			}
			if (depositHeaders) {
				for (Map.Entry<String, String> entry : headers.entrySet()) {
					put.add(cf, entry.getKey().getBytes(charset), entry
							.getValue().getBytes(charset));
				}
			}
			actions.add(put);
		} catch (Exception e) {
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
