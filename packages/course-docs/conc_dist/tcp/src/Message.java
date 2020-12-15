import java.net.*;
import java.io.*;
import java.util.*;
import java.io.Serializable;

public abstract class Message implements Serializable {   
    private static final long serialVersionUID = 1L;
    public String message; 
    public Message(String message) {
        this.message = message;
    }
}

class ClientRequest extends Message {
    public ClientRequest(String request) {
        super(request);
    }
}

class ServerConnection extends Message {
    public ServerConnection(String fromId) {
        super(fromId);
    }
}

class ServerRequest extends Message {
    public Integer logicalClock;
    public Integer pid;
    public ServerRequest(Integer logicalClock, Integer pid, String request) {
        super(request);
        this.logicalClock = logicalClock;
        this.pid = pid;
    }
    public static int compare(ServerRequest a, ServerRequest b) {
        if (a.logicalClock > b.logicalClock) return 1;
        if (a.logicalClock <  b.logicalClock) return -1;
        if (a.pid > b.pid) return 1;
        if (a.pid < b.pid) return -1;
        return 0;
    }
    public int getLogicalClock() {
        return logicalClock;
    }
    public int getPid() {
        return pid;
    }
}

class Acknowledgement extends Message {
    public Integer logicalClock;
    public Acknowledgement(String request, Integer logicalClock) {
        super(request);
        this.logicalClock = logicalClock;
    }
}

class Release extends Message {
    public Integer logicalClock;
    public Integer pid;
    public Release(String release, Integer logicalClock) {
        super(release);
        this.logicalClock = logicalClock;
    }
}

class ServerResponse extends Message {
    public String updateNeeded;
    public ServerResponse(String response) {
        super(response); 
        this.updateNeeded = null; 
    }
}

class Update extends Message {
    public HashMap<String,Integer> inventory;
    public HashMap<Integer,ArrayList<String>> orders;
    public HashMap<String,ArrayList<Integer>> customers;
    public ArrayList<Integer> orderIds;
    public Update(String response) {
        super(response);
        this.inventory = Server.store.getInventory();
        this.orders = Server.store.getOrders();
        this.customers = Server.store.getCustomers();
        this.orderIds = Server.store.getOrderIds();
    }
}