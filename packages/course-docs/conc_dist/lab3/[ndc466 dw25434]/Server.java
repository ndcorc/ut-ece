public class Server {
  public static void main (String[] args) {

	  int tcpPort;
	  int udpPort;
    
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
	  
	  /*
	  String fileName = "inventory.txt";
	  tcpPort = 3000;
	  udpPort = 8000;
	  */
	  // parse the inventory file	
	  Store store = new Store();
	  store.initializeInventory(fileName);
	  

	  UdpServer udp = new UdpServer(udpPort, store);
	  udp.start();

	  TcpServer tcp = new TcpServer(tcpPort, store);
	  tcp.start();
  }
}