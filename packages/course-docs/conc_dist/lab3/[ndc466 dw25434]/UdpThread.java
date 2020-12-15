import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;

public class UdpThread implements Runnable {
    private final Store store;
    private final DatagramSocket datasocket;
    private final DatagramPacket datapacket;
    private String output;
    private String cmd;

    public UdpThread(DatagramPacket datapacket, DatagramSocket datasocket, Store store) {
      this.store = store;
      this.datasocket = datasocket;
      this.datapacket = datapacket;
    }
    public void run() {
    	
          cmd = new String(datapacket.getData());
		  String[] tokens = cmd.split(" ");
		  if (tokens[0].equals("purchase")) {
			  String userName = tokens[1];
			  String productName = tokens[2];
			  Integer quantity = Integer.parseInt(tokens[3]);
			  output = store.purchase(userName, productName, quantity);
		  } else if (tokens[0].equals("cancel")) {
			  Integer orderId = Integer.valueOf(tokens[1]);
			  output = store.cancel(orderId);
		  } else if (tokens[0].equals("search")) {
			  String userName = tokens[1];
			  output = store.search(userName);
		  } else if (tokens[0].equals("list")) {
			  output = store.list();
		  }
		  
		  try {
			  byte[] rbuffer = new byte[output.length()];
			  rbuffer = output.getBytes();
			  DatagramPacket returnpacket = new DatagramPacket(
						  		rbuffer,
						  		rbuffer.length,
						  		datapacket.getAddress(),
						  		datapacket.getPort());
			  datasocket.send(returnpacket);
		  } catch (IOException e) {
			  e.printStackTrace();
		  }
    }
}
