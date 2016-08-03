package velassco.datainjection.util;

import java.beans.BeanInfo;
import java.beans.Introspector;
import java.beans.PropertyDescriptor;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringWriter;
import java.io.UnsupportedEncodingException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Properties;
import java.util.UUID;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import org.apache.solr.client.solrj.SolrQuery;
import org.apache.solr.client.solrj.SolrServerException;
import org.apache.solr.client.solrj.impl.CommonsHttpSolrServer;
import org.apache.solr.client.solrj.impl.StreamingUpdateSolrServer;
import org.apache.solr.client.solrj.impl.XMLResponseParser;
import org.apache.solr.client.solrj.response.QueryResponse;
import org.apache.solr.client.solrj.response.UpdateResponse;
import org.apache.solr.client.solrj.util.ClientUtils;
import org.apache.solr.common.SolrDocument;
import org.apache.solr.common.SolrDocumentList;
import org.apache.solr.common.SolrInputDocument;
import org.apache.solr.common.params.ModifiableSolrParams;
import org.apache.solr.search.QueryUtils;


public class SolrManager {

	private static SolrManager instance = null;
	
	protected SolrManager() {
		Properties properties = new Properties();
		try {
			properties.load(this.getClass().getResourceAsStream("solr.properties"));
			String solrServer = properties.getProperty("solr.server");
			String distDay = properties.getProperty("solr.distDay");
			String distHist = properties.getProperty("solr.distHist");
			String distHistAll = properties.getProperty("solr.distHistAll");
			//server = new CommonsHttpSolrServer(HTTP_PREFIX + solrServer + "/" + distDay);
			//server.setParser(new XMLResponseParser());
			serverDay = this.initSolrIndex(HTTP_PREFIX + solrServer + "/" + distDay);
			serverHis = this.initSolrIndex(HTTP_PREFIX + solrServer + "/" + distHist);
			//shards = solrServer + "/" + distDay + "," + solrServer + "/" + distHist + "," + solrServer + "/" + distHistAll;
			shards = solrServer + "/" + distHist;
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static SolrManager getInstance() {
	  if(instance == null) {
	     instance = new SolrManager();         
	  }
	  return instance;
	}
	
	@SuppressWarnings("deprecation")
	//private static CommonsHttpSolrServer server;
	private StreamingUpdateSolrServer serverHis, serverDay = null;
	private static String shards;
	private static final String HTTP_PREFIX = "http://";
	
	private StreamingUpdateSolrServer initSolrIndex(String indexUrl) throws MalformedURLException,
	  SolrServerException, IOException {
	  StreamingUpdateSolrServer server = new StreamingUpdateSolrServer(indexUrl, 100, 5);
	  server.setParser(new XMLResponseParser());
	  return server;
	}
       
    
}
