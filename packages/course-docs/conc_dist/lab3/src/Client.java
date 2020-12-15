import java.net.*;
import java.io.*;
import java.util.*;

public class Client {
    public static void main (String[] args) {
    	  
        String hostAddress;
        String protocolMode = "T";
        InetAddress ia;
        
        // Datagram client variables for UDP protocol
        DatagramSocket datasocket;

        // Name server client variables for TCP protocol
        ObjectInputStream input = null;
        ObjectOutputStream output = null;
        Socket server;

        int tcpPort;
        int udpPort; 
/*
        if (args.length != 3) {
          System.out.println("ERROR: Provide 3 arguments");
          System.out.println("\t(1) <hostAddress>: the address of the server");
          System.out.println("\t(2) <tcpPort>: the port number for TCP connection");
          System.out.println("\t(3) <udpPort>: the port number for UDP connection");
          System.exit(-1);
        }
        
        hostAddress = args[0];
        tcpPort = Integer.parseInt(args[1]);
        udpPort = Integer.parseInt(args[2]);
*/

        hostAddress = "10.184.69.105";
        tcpPort = 5000;
        udpPort = 8000;

        try {
    	    @SuppressWarnings("resource")
    		Scanner sc = new Scanner(System.in);
    	    ia = InetAddress.getByName(hostAddress);
    	    datasocket = new DatagramSocket();
            datasocket.setSoTimeout(100);
    	    UdpClient udp = new UdpClient(ia, datasocket, udpPort);
    	    
    	    server = new Socket(ia, tcpPort);

            try {
                output = new ObjectOutputStream(server.getOutputStream());
                input = new ObjectInputStream(server.getInputStream());
            } catch (IOException e) {}

    	    TcpClient tcp = new TcpClient(output, input);
    	    
            System.out.println("Current prococol: " + protocolMode);
    	    while(sc.hasNextLine()) {
                String cmd = sc.nextLine();
                String[] tokens = cmd.split(" ");
                if (tokens[0].equals("setmode")) {
                    protocolMode = tokens[1];
                    System.out.println("Current protocol: " + protocolMode);
                }
                else {
                    if (protocolMode == "T") {
                        tcp.execute(cmd);
                    }
                    else {
                        udp.execute(cmd);
                    }
                }    
    	    }
        } catch (IOException e) {
        	e.printStackTrace();
        }
    }
}