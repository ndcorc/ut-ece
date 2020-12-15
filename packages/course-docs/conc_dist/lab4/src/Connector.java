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
	//public ServerSocket listener;
    public HashMap<Integer, InetSocketAddress> neighbors;
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
			} catch (Exception e) {}
		}
		return socket;
	}
	
	/* SERVER -> SERVER */
	public void connect(InetSocketAddress myServer) {	
        this.connections = new HashMap<Integer, Socket>();
        this.inputStreams = new HashMap<Integer, ObjectInputStream>();
        this.outputStreams = new HashMap<Integer, ObjectOutputStream>();
        this.myServer = myServer;
/*
        try {
            this.listener = new ServerSocket(myServer.getPort());
        } catch (IOException e) {
            e.printStackTrace();
        }
*/
        /* Contact all the bigger processes */
        for (Integer pid : neighbors.keySet()) {
            if (pid > myId) {
                InetSocketAddress neighborAddress = neighbors.get(pid);
                Socket neighbor = null;
                while (neighbor == null) {
	                try {
	                	neighbor = new Socket(neighborAddress.getAddress(), neighborAddress.getPort());
	                	System.out.println("Connected with server " + pid + " at: " + neighborAddress.toString());
	                } catch (IOException e) {}
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

    public void listen(HashMap<Integer, InetSocketAddress> neighbors, Integer id) {
        /* Accept connections from all the smaller processes */
        this.myId = id;
        this.neighbors = neighbors;
        this.connections = new HashMap<Integer, Socket>();
        this.inputStreams = new HashMap<Integer, ObjectInputStream>();
        this.outputStreams = new HashMap<Integer, ObjectOutputStream>();
        System.out.println("Total neighbors: " + neighbors.size());
        for (int i = 1; i < neighbors.size(); i++) {
            try {
                Socket neighbor = Server.listener.accept();
                ObjectOutputStream output = new ObjectOutputStream(neighbor.getOutputStream());
                ObjectInputStream input = new ObjectInputStream(neighbor.getInputStream());
                Integer pid = Integer.parseInt((String) input.readObject());
                System.out.println("Neighbor: " + pid);
                connections.put(pid, neighbor);
                inputStreams.put(pid, input);
                outputStreams.put(pid, output);
            } catch (Exception e) {}
        }
    }

	public void closeSockets() {
        try {
            Server.listener.close();
            for (Map.Entry<Integer, Socket> connection : connections.entrySet()) {
            	connection.getValue().close();
            }
        } catch (Exception e) {
        }
	}
}
