package velassco.datainjection.bean;

import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlTransient;


@XmlRootElement
public class MessageData {
	
	@XmlTransient
	private String _global_id;

	public String get_global_id() {
		return _global_id;
	}

	public void set_global_id(String _global_id) {
		this._global_id = _global_id;
	}
	
}
