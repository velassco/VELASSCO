struct message 
{
  1: string message1,
  2: optional string message2,
  3: optional string message3,
  4: optional string message4,
  5: optional string message5,
  6: optional string message6
}



exception InvalidOperation {
  1: i32 what,
  2: string why
}

service ServerService {
	message getStatusOfAllNodes()  throws (1:InvalidOperation ouch),
	message getStatusOfNode(1: string nodeName)  throws (1:InvalidOperation ouch),
	
	message getStatusOfAllServicesOnNode(1: string nodeName)  throws (1:InvalidOperation ouch),
	message getStatusOfServiceOnNode(1: string nodeName, 2: string serviceName)  throws (1:InvalidOperation ouch),
	
	oneway void startAllServicesOnAllNodes() ,
	oneway void startAllServicesOnANode(1: string nodeName),
	oneway void startAServiceOnANode(1: string nodeName, 2: string serviceName) ,

	oneway void stopAllServicesOnAllNodes(),
	oneway void stopAllServicesOnANode(1: string nodeName)  ,
	oneway void stopAServiceOnANode(1: string nodeName, 2: string serviceName) ,
	
	message getGDFConfigurationFile(1: string fileName)  throws (1:InvalidOperation ouch),
	void setGDFConfigurationFile(1: string fileName, 2: string message) throws (1:InvalidOperation ouch),
	
	message query(1: message querry)  throws (1:InvalidOperation ouch),
	
	message ping()
}