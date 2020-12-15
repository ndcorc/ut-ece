import java.net.*;
import java.io.*;
import java.util.*;

public class Server {

    /* SERVER INFO */
  	public static Store store;
    public static InetSocketAddress myServer;
    public static Integer myId;
    public static Integer numServer;
    public static Integer N;
    public static ServerSocket listener;
    public static Lamport lamport;

    /* SERVER NETWORK INFO */
    public static HashMap<Integer, InetSocketAddress> neighbors = new HashMap<Integer, InetSocketAddress>();
    public static HashMap<Integer, Socket> connections = new HashMap<Integer, Socket>(); // INCOMING SERVERS
    public static HashMap<Integer, ObjectOutputStream> outputStreams = new HashMap<Integer, ObjectOutputStream>();
    public static HashMap<Integer, ObjectInputStream> inputStreams = new HashMap<Integer, ObjectInputStream>();

  	public static void main (String[] args) {
        Settings.initServer(args);
        try { listener = new ServerSocket(myServer.getPort(), 0, myServer.getAddress()); } 
        catch (IOException e) {}

        Linker linker = new Linker();
        lamport = new Lamport(linker);
        linker.init();
      	try {
        	Socket socket;
          	while ((socket = listener.accept()) != null) {  // LISTEN FOR CLIENT
            	new Thread(new ClientHandler(socket)).start();
            }
      	} catch (Exception e) {
        	System.exit(-1);
     	}
  	}
}