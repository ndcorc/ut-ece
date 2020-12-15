import java.net.*;
import java.io.*;
import java.util.*;

public class Client {
    
    public static final Boolean DEBUG = true;
    public static Socket server = null;
    public static ArrayList<InetSocketAddress> servers = new ArrayList<InetSocketAddress>();

    public static void connect() {  // CONNECT TO CLIENT
        try { server = new Socket(servers.get(0).getAddress(), servers.get(0).getPort()); } // unique port
        catch (Exception e) {
            System.out.println("\nServer at " + servers.get(0).getHostName() + ":" + servers.get(0).getPort() + " not running");
            servers.remove(0);
            if (servers.size() == 0) System.exit(-1);
            connect();
        }
    }

	public static void main (String[] args) {
		Scanner sc = new Scanner(System.in);
		Settings settings = new Settings(args);
        if (DEBUG) {
        	settings.initClientFiles();
        }
        else {
        	settings.initClient(sc);
        }
	    Connector connector = new Connector();
	    connect();
		ObjectInputStream input = null;
		ObjectOutputStream output = null;
        try {
            output = new ObjectOutputStream(server.getOutputStream());
            input = new ObjectInputStream(server.getInputStream());
        } catch (IOException e) {}
        Boolean newConnection = false;

        System.out.print("Command: ");
	    while(sc.hasNextLine()) {
	    	String cmd = sc.nextLine();
	    	String[] tokens = cmd.split(" ");
	    	switch(tokens[0]) {
	    		case "purchase":
	    		case "cancel":
	    		case "search":
	    		case "list":
			        while (true) {
			            try {
			                if (newConnection) {
			                    try {
			                        output = new ObjectOutputStream(server.getOutputStream());
			                        input = new ObjectInputStream(server.getInputStream());
			                    } catch (IOException e) {}
			                    newConnection = false;
			                }
			                output.writeObject(new ClientRequest(cmd));
			                output.flush();
			                ServerResponse response = (ServerResponse) input.readObject();
			                System.out.println(response.message);
			                break;
			            } catch (Exception e) {
			                System.out.println("\nConnection to server " + servers.get(0).toString() + " has been lost ...");
			                settings.servers.remove(0);
			                connect();
			                newConnection = true;
			            }
			        }
		    		break;
	    		default:
	    			System.out.println("ERROR: No such command");
	    	} 
	    	System.out.print("Command: ");		   
	    }
	    sc.close();
	}
	
}