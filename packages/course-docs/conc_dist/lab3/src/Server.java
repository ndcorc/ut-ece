import java.net.*;
import java.io.*;
import java.util.*;

public class Server {

	public static Store store;
	public static InetAddress locIP;
	public static ServerSocket listener;

  	public static void main (String[] args) {

	  	int tcpPort;
	  	int udpPort;
/*
	  if (args.length != 3) {
		  System.out.println("ERROR: Provide 3 arguments");
		  System.out.println("\t(1) <tcpPort>: the port number for TCP connection");
		  System.out.println("\t(2) <udpPort>: the port number for UDP connection");
		  System.out.println("\t(3) <file>: the file of inventory");
		  System.exit(-1);
	  }
	  tcpPort = Integer.parseInt(args[0]);
	  udpPort = Integer.parseInt(args[1]);
	  String fileName = args[2];
*/  
	  	String fileName = "inventory.txt";
	  	tcpPort = 5000;
	  	udpPort = 8080;

	  	// parse the inventory file	
	  	store = new Store(fileName);
	  

	  	UdpServer udp = new UdpServer(udpPort, store);
	  	udp.start();

	  	InetSocketAddress ia = new InetSocketAddress("10.184.69.105", 5000);
      	try {
       		listener = new ServerSocket(ia.getPort(), 0, ia.getAddress());
      	} catch (IOException e) {
    	  	e.printStackTrace();
    	  	System.exit(-1);
      	}
      	
      	try {
      		Socket socket;
        	while ((socket = listener.accept()) != null) {
        		System.out.println("New client has connected");
          		new Thread(new TcpThread(socket)).start();
        	}
      	} catch (IOException e) {
        	e.printStackTrace();
        	System.exit(-1);
     	}
  	}
}