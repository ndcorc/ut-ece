import java.net.*;
import java.io.*;
import java.util.*;

public class Client {

    public static ArrayList<InetSocketAddress> servers = new ArrayList<InetSocketAddress>();
    public static Socket server = null;
    public static ObjectOutputStream output = null;
    public static ObjectInputStream input = null;

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
        Settings.initClient(args);
        connect();
        try {
            output = new ObjectOutputStream(server.getOutputStream());
            input = new ObjectInputStream(server.getInputStream());
        } catch (IOException e) {}
        Boolean newConnection = false;
        Scanner sc = new Scanner(System.in);
	    while(sc.hasNextLine()) {
            String cmd = sc.nextLine();
            String[] tokens = cmd.split(" ");
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
                server.setSoTimeout(100);
                ServerResponse response = (ServerResponse) input.readObject();
                System.out.println(response.message);
            } catch (Exception e) {
                System.out.println("\nConnection to server " + servers.get(0).toString() + " has been lost ...");
                connect();
                newConnection = true;
            }
        }     
    }
}