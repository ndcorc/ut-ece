import java.util.*;
import java.net.*;
import java.io.*;
import java.util.concurrent.locks.*;

public class Linker {
    
    public InetSocketAddress myServer;
    public Integer myId;
    public HashMap<Integer, InetSocketAddress> neighbors;
    public Integer N;
    public Connector connector;

    public final Lock lock = new ReentrantLock();
    public Condition condition = lock.newCondition();

    public Linker() {
        this.connector = new Connector();
        this.connector.connect();
    }

    public void init() {
        for (Integer pid : Server.neighbors.keySet()) {
            System.out.println("Listening for messages from server " + pid + "\n");
        	new Thread(new ServerHandler(pid, this)).start();
        }
    }
    
    public void sendMsg(Integer destId, Message message) throws Exception {

        Socket connection = Server.connections.get(destId);
        synchronized(connection) {
            try {
                ObjectOutputStream output = Server.outputStreams.get(destId);
                output.writeObject(message);
                output.flush();
            } catch (IOException e) {
                connection.close();
                Server.inputStreams.get(destId).close();
            } 
        }
    }
    
    public Message receiveMsg(Integer fromId) throws Exception {
        Message message = null;
        ObjectInputStream input = null;
        try {
            input = Server.inputStreams.get(fromId);
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