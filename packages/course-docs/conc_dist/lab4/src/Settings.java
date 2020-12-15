import java.util.*;
import java.net.*;
import java.io.*;

public class Settings {

	private String[] args;
    private Integer numServer;
    private HashMap<Integer, InetSocketAddress> neighbors;

    public ArrayList<InetSocketAddress> servers;
    private Integer myId;
    private InetSocketAddress myServer;
    private String inventory;

	public Settings(String[] args) {
		this.args = args;
	}

	public void initClient(Scanner sc) {
		servers = new ArrayList<InetSocketAddress>();
	    int numServer = Integer.parseInt(sc.nextLine());
		String config;
		String[] tokens;
	    for (int i = 0; i < numServer; i++) {
	    	config = sc.nextLine();
	        tokens = config.split(":");
	        servers.add(new InetSocketAddress(tokens[0], Integer.parseInt(tokens[1])));
	    }
	}

	public void initClientFiles() {
		servers = new ArrayList<InetSocketAddress>();
		String config;
		String[] tokens;
        if (args.length != 1) {
            System.out.println("ERROR: Provide 1 arguments");
            System.exit(-1);
        }
        String fileName = "../" + args[0];
        String line = null;
        String inventory = null;
        BufferedReader input;
        try {
        	input = new BufferedReader(new FileReader(fileName));
            int numServer = Integer.parseInt(input.readLine());

		    for (int i = 0; i < numServer; i++) {
		    	config = input.readLine();
		        tokens = config.split(":");
		        servers.add(new InetSocketAddress(tokens[0], Integer.parseInt(tokens[1])));
		    }
		    input.close();            
        } catch (IOException e) {}
	}

	public void initServer() {

        Scanner sc = new Scanner(System.in);
       	neighbors = new HashMap<Integer, InetSocketAddress>();
        myId = Integer.parseInt(sc.nextLine());
        numServer = Integer.parseInt(sc.nextLine());
		inventory = sc.nextLine();

		printSettings();
        
        for (int i = 0; i < numServer; i++) {
            String config = sc.nextLine();
            String[] tokens = config.split(":");
            System.out.println("Server " + (i+1) + " - " + config);

            if (i+1 == myId) {
            	myServer = new InetSocketAddress(tokens[0], Integer.parseInt(tokens[1]));
            } else {
                neighbors.put(i+1, new InetSocketAddress(tokens[0], Integer.parseInt(tokens[1])));
            }
        }
        sc.close();
        System.out.println("");
	}

	public void initServerFiles() {
        if (args.length != 1) {
            System.out.println("ERROR: Provide 1 arguments");
            System.exit(-1);
        }
        neighbors = new HashMap<Integer, InetSocketAddress>();
        String fileName = "../" + args[0];
        BufferedReader input = null;
        try {
            input = new BufferedReader(new FileReader(fileName));
	        myId = Integer.parseInt(input.readLine());
	        numServer = Integer.parseInt(input.readLine());
	        inventory = input.readLine();
	    } catch (IOException e) {
			System.out.println("ERROR: Inventory file not found");
		}

        printSettings();
        try {
	        for (int i = 0; i < numServer; i++) {
	            	String config = input.readLine();
	            	String[] tokens = config.split(":");
	            	System.out.println("Server " + (i+1) + " - " + config);
	          		if (i+1 == myId) {
		                myServer = new InetSocketAddress(tokens[0], Integer.parseInt(tokens[1]));
			       	} else {
			       		neighbors.put(i+1, new InetSocketAddress(tokens[0], Integer.parseInt(tokens[1])));
			        }
			}
			input.close();
		} catch (IOException e) {
	      	e.printStackTrace();
        }
        System.out.println("");
	}

	public void printSettings() {
        System.out.println("\n\nServer ID: " + myId);
        System.out.println("Number of server instances: " + numServer);
        System.out.println("Inventory path: " + inventory);
        System.out.println("\nNetwork settings:");
	}

	public String getInventory() {
		return inventory;
	}

	public ArrayList<InetSocketAddress> getServers() {
		return servers;
	}

    public Integer getNumServer() {
    	return numServer;
    }

    public HashMap<Integer, InetSocketAddress> getNeighbors() {
    	return neighbors;
    }

    public Integer getMyId() {
    	return myId;
    }

    public InetSocketAddress getMyServer() {
    	return myServer;
    }

}