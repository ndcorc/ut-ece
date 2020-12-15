import java.io.IOException;
import java.net.*;

public class TcpServer extends Thread {
	
    private ServerSocket listener;
    private Socket socket;
    private Store store;
    private InetSocketAddress ia;
    
    TcpServer(int port, Store store) {
      this.store = store;
      try {
        InetAddress locIP = InetAddress.getByName("10.184.69.105");
        this.listener = new ServerSocket(5000, 0, locIP);
        //this.listener = new ServerSocket();
        //this.listener.bind(ia);
      } catch (IOException e) {
    	  e.printStackTrace();
    	  System.exit(-1);
      }
    }

    @Override
    public void run() {
      try {
        while ( (socket = listener.accept()) != null) {
          Thread t = new Thread(new TcpThread(socket));
          t.start();
        } 
      } catch (IOException e) {
        e.printStackTrace();
        return;
      }
    }
  }
