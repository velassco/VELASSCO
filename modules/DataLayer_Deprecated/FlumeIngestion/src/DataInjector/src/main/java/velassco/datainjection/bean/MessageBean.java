package velassco.datainjection.bean;

import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlTransient;
import javax.persistence.Entity;


@XmlRootElement(name="message")
public class MessageBean { 

	@XmlTransient
	private String _timeExec;
	
	public String get_timeExec() {
		return _timeExec;
	}

	public void set_timeExec(String _timeExec) {
		this._timeExec = _timeExec;
	}

	@XmlTransient
	private String _messageState;

	@XmlElement(name = "state")
	public String get_messageState() {
		return _messageState;
	}

	public void set_messageState(String _messageState) {
		this._messageState = _messageState;
	}
	
	@XmlTransient
	private MessageData _messageData;

	@XmlElement(name = "data")
	public MessageData get_messageData() {
		return _messageData;
	}

	public void set_messageData(MessageData _messageData) {
		this._messageData = _messageData;
	}
		
}