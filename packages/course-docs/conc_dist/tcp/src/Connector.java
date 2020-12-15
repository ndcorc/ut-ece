import java.net.*;
import java.io.*;
import java.util.*;

public class Connector {
	
    /* SERVER -> SERVER */
    public void connect() {    
        /* Accept connections from all the smaller processes */
        for (int i = 0; i < Server.myId-1; i++) {
            try {
                Socket neighbor = Server.listener.accept();
                ObjectInputStream input = new ObjectInputStream(neighbor.getInputStream());
                ObjectOutputStream output = new ObjectOutputStream(neighbor.getOutputStream());
                Integer pid = Integer.parseInt((String) input.readObject());
                System.out.println("Neighbor: " + pid);
                Server.connections.put(pid, neighbor);
                Server.inputStreams.put(pid, input);
                Server.outputStreams.put(pid, output);
            } catch (Exception e) {}
        }

        /* Contact all the bigger processes */
        for (Integer pid : Server.neighbors.keySet()) {
            if (pid > Server.myId) {
                InetSocketAddress neighborAddress = Server.neighbors.get(pid);
                Socket neighbor = null;
                while (neighbor == null) {
                    try {
                        neighbor = new Socket(neighborAddress.getAddress(), neighborAddress.getPort());
                        System.out.println("Connected with server " + pid + " at: " + neighborAddress.toString());
                    } catch (IOException e) {}
                }
                try {
                    Server.connections.put(pid, neighbor);
                    Server.outputStreams.put(pid, new ObjectOutputStream(neighbor.getOutputStream()));
                    Server.inputStreams.put(pid, new ObjectInputStream(neighbor.getInputStream()));

                    Server.outputStreams.get(pid).writeObject(new String("" + Server.myId));
                    Server.outputStreams.get(pid).flush();
                } catch (IOException e) {
                    System.out.println("Could not send message to neighbor");
                }
            } 
        }
        return;
    }

    public void closeSockets() {
        try {
            Server.listener.close();
            for (Map.Entry<Integer, Socket> connection : Server.connections.entrySet()) {
                connection.getValue().close();
            }
        } catch (Exception e) {}
    }
}