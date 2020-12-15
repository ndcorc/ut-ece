import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.net.Socket;

public class TcpThread implements Runnable {
	
    private  Store store;
    private Socket socket;
    private String output;
    private String cmd;

    public TcpThread(Store store, Socket socket) {
      this.store = store;
      this.socket = socket;
    }

    @Override
    public void run() {
      try {
        BufferedReader inputStream = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        PrintStream outputStream = new PrintStream(socket.getOutputStream());

        while (true) {
          String message = inputStream.readLine();
          if (message == null)
            break;

          // Perform task
          cmd = inputStream.readLine();
          if (cmd == null)
              break;
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
		  
		  output += "\ndone";
          // Send reply
          outputStream.println(output);
          outputStream.flush();
        }
      } catch (IOException e) {
    	  e.printStackTrace();
      }
    }
}
