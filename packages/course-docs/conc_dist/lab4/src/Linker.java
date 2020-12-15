import java.util.*;
import java.net.*;
import java.io.*;
import java.util.concurrent.locks.*;

public class Linker implements Runnable {
    
    public InetSocketAddress myServer;
    public Integer myId;
    public HashMap<Integer, InetSocketAddress> neighbors;
    public Integer N;
    public Connector connector;

    public final Lock lock = new ReentrantLock();
    public Condition condition = lock.newCondition();

    public Linker(Settings settings) {
        this.myServer = settings.getMyServer();
        this.myId = settings.getMyId();
        this.neighbors = settings.getNeighbors();
        this.N = neighbors.size();
        this.connector = new Connector();
        this.connector.listen(neighbors, myId);
    }
    public void run() {
        this.connector.connect(myServer);
        for (Integer pid : neighbors.keySet()) {
            System.out.println("Listening for messages from server " + pid + "\n");
        	ListenerThread lt = new ListenerThread(pid, this);
        	lt.start();
        }
    }
    
    public void sendMsg(Integer destId, Message message) throws Exception {

        Socket connection = connector.connections.get(destId);
        synchronized(connection) {
            try {
                ObjectOutputStream output = connector.outputStreams.get(destId);
                output.writeObject(message);
                output.flush();
            } catch (IOException e) {
                connection.close();
                connector.inputStreams.get(destId).close();
            } 
        }
    }
    
    public Message receiveMsg(Integer fromId) throws Exception {
        Message message = null;
        ObjectInputStream input = null;
        try {
            input = connector.inputStreams.get(fromId);
            message = (Message) input.readObject();
        } catch (Exception e) {
            input.close();
            throw new Exception();
        }
        return message;
    }
    
    public void close() {
        connector.closeSockets();
    }

    public Integer getMyId() {
        return myId;
    }
}