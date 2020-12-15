public class ServerSettings {
	private int serverId;
	private String host;
	private int port;
	
	public ServerSettings(int serverId) {
		this.serverId = serverId;
	}

	public ServerSettings(String host, int port) {
		this.host = host;
		this.port = port;
	}

	public ServerSettings(int serverId, String host, int port) {
		this.serverId = serverId;
		this.host = host;
		this.port = port;		
	}
	
	public void addAddress(String host) {
		this.host = host;
	}
	
	public void addPort(int port) {
		this.port = port;
	}

	public int getId() {
		return serverId;
	}


}
