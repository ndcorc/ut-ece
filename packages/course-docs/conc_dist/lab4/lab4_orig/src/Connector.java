import java.util.*;
import java.net.*;
import java.io.*;

public class Connector {

	private Socket connection;
	private String serverSettings;

	public ObjectInputStream input;
	public ObjectOutputStream output;

	public InetSocketAddress myServer;
	public Integer myId;
	public ServerSettings thisServer;
	public ServerSocket listener;
	public HashMap<Integer, Socket> connections;
	public HashMap<Integer, ObjectOutputStream> outputStreams;
	public HashMap<Integer, ObjectInputStream> inputStreams;

	public static final int TIMEOUT = 100;	
	
	/* CLIENT -> SERVER */
	public Socket connect(ArrayList<InetSocketAddress> servers) {	
		Socket socket = null;
		while(socket == null) {
			try {
				socket = new Socket(servers.get(0).getAddress(), servers.get(0).getPort());
				System.out.println("Connected to server at " + servers.get(0).toString() + "\n");
			} catch (Exception e) {
				System.out.println("All servers have crashed");
				System.exit(-1);
			}
		}
		return socket;
	}
	
	/* SERVER -> SERVER */
	public void connect(HashMap<Integer, InetSocketAddress> neighbors, InetSocketAddress myServer, Integer id) {	
        this.connections = new HashMap<Integer, Socket>();
        this.inputStreams = new HashMap<Integer, ObjectInputStream>();
        this.outputStreams = new HashMap<Integer, ObjectOutputStream>();
        this.myId = id;
        this.myServer = myServer;

        try {
            this.listener = new ServerSocket(myServer.getPort());
        } catch (IOException e) {
            e.printStackTrace();
        }

        /* Accept connections from all the smaller processes */
        for (int i = 0; i < myId-1; i++) {
            try {
                Socket neighbor = listener.accept();
                ObjectInputStream input = new ObjectInputStream(neighbor.getInputStream());
                ObjectOutputStream output = new ObjectOutputStream(neighbor.getOutputStream());
                Integer pid = Integer.parseInt((String) input.readObject());
                System.out.println("Neighbor: " + pid);
                connections.put(pid, neighbor);
                inputStreams.put(pid, input);
                outputStreams.put(pid, output);
            } catch (Exception e) {}
        }

        /* Contact all the bigger processes */
        for (Integer pid : neighbors.keySet()) {
            if (pid > myId) {
                InetSocketAddress neighborAddress = neighbors.get(pid);
                Socket neighbor = null;
                while (neighbor == null) {
	                try {
	                	neighbor = new Socket(neighborAddress.getAddress(), neighborAddress.getPort());
	                	System.out.println("Connected with server " + pid + " at: " + neighborAddress.toString());
	                } catch (IOException e) {
	                }
	            }
                try {
                    connections.put(pid, neighbor);
                    outputStreams.put(pid, new ObjectOutputStream(neighbor.getOutputStream()));
                   	inputStreams.put(pid, new ObjectInputStream(neighbor.getInputStream()));

                    outputStreams.get(pid).writeObject(new String("" + myId));
                    outputStreams.get(pid).flush();
                } catch (IOException e) {
                    System.out.println("Could not send message to neighbor");
                }
            } 
        }
        return;
    }

	public void closeSockets() {
        try {
            listener.close();
            for (Map.Entry<Integer, Socket> connection : connections.entrySet()) {
            	connection.getValue().close();
            }
        } catch (Exception e) {}
	}
}
