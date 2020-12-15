import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

class UdpServer extends Thread {
    
	int bufferLen = 1024;
    private Store store;
    private DatagramSocket datasocket;
    private byte[] buffer;

    
    
    UdpServer(int port, Store store) {
      this.store = store;
      this.buffer = new byte[bufferLen];
      try {
        this.datasocket = new DatagramSocket(port);
      } catch (SocketException e) {
    	  e.printStackTrace();
      }
    }
    

    @Override
    public void run() {
      DatagramPacket datapacket;

      while (true) {
        datapacket = new DatagramPacket(buffer, buffer.length);
        try {	
          datasocket.receive(datapacket);
          Thread listener = new Thread(new UdpThread(datapacket, datasocket, store));
          listener.start();
        } catch (IOException e) {
        	e.printStackTrace();
        	datasocket.close();
        	return;
        }
      }
    }
  }