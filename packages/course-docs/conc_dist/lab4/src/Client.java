import java.net.*;
import java.io.*;
import java.util.*;

public class Client {
    
    public static final Boolean DEBUG = true;

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
	    Socket connection = connector.connect(settings.getServers());
		ObjectInputStream input = null;
		ObjectOutputStream output = null;
        try {
       	    output = new ObjectOutputStream(connection.getOutputStream());
            input = new ObjectInputStream(connection.getInputStream());
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
			            	if (newConnection == true) {
			            		output = new ObjectOutputStream(connection.getOutputStream());
			            		input = new ObjectInputStream(connection.getInputStream());
            					newConnection = false;
			            	}
			                output.writeObject(new ClientRequest(cmd));
			                output.flush();
			                ServerResponse response = (ServerResponse) input.readObject();
			                System.out.println(response.message);
			                break;
			            } catch (Exception e) {
			                System.out.println("\nConnection to server " + settings.servers.get(0).toString() + " has been lost ...");
			                settings.servers.remove(0);
			                if (settings.servers.size() == 0) {
			                	System.out.println("All servers have crashed");
								System.exit(-1);
			                }
			                connection = connector.connect(settings.servers);
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