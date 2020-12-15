import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class TcpServer extends Thread {
	
    private ServerSocket socket;
    private Store store;
    
    TcpServer(int port, Store store) {
      this.store = store;

      try {
        this.socket = new ServerSocket(port);
      } catch (IOException e) {
    	  e.printStackTrace();
    	  System.exit(-1);
      }
    }

    @Override
    public void run() {
      while (true) {
        try {
          Socket datasocket = socket.accept();

          Thread listener = new Thread(new TcpThread(store, datasocket));
          listener.start();
        } catch (IOException e) {
        	e.printStackTrace();
        	return;
        }

      }
    }
  }
