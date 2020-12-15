import java.net.*;
import java.io.*;
import java.util.*;

public class Settings {

	public static void initServer(String[] args) {
        if (args.length != 1) {
            System.out.println("ERROR: Provide 1 arguments");
            System.exit(-1);
        }
        String fileName = "../" + args[0];
        BufferedReader input = null;
        try {
            input = new BufferedReader(new FileReader(fileName));
            Server.myId = Integer.parseInt(input.readLine());
            Server.numServer = Integer.parseInt(input.readLine());
            Server.store = new Store(input.readLine());
        } catch (IOException e) {
            System.out.println("ERROR: Inventory file not found");
        }
        System.out.println("\n\nServer ID: " + Server.myId);
        System.out.println("Number of server instances: " + Server.numServer);
        System.out.println("\nNetwork settings:");
        try {
            for (int i = 0; i < Server.numServer; i++) {
                    String config = input.readLine();
                    String[] tokens = config.split(":");
                    System.out.println("Server " + (i+1) + " - " + config);
                    if (i+1 == Server.myId) { Server.myServer = new InetSocketAddress(tokens[0], Integer.parseInt(tokens[1])); } 
                    else { Server.neighbors.put(i+1, new InetSocketAddress(tokens[0], Integer.parseInt(tokens[1]))); }
            }
            Server.N = Server.neighbors.size();
            input.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("");
    }

	public static void initClient(String[] args) {
        String config;
        String[] tokens;
        if (args.length != 1) {
            System.out.println("ERROR: Provide 1 arguments");
            System.exit(-1);
        }
        String fileName = "../" + args[0];
        String line = null;
        String inventory = null;
        BufferedReader settings;
        try {
            settings = new BufferedReader(new FileReader(fileName));
            int numServer = Integer.parseInt(settings.readLine());
            for (int i = 0; i < numServer; i++) {
                config = settings.readLine();
                tokens = config.split(":");
                Client.servers.add(new InetSocketAddress(tokens[0], Integer.parseInt(tokens[1])));
            }
            settings.close();            
        } catch (IOException e) {}
	}

}