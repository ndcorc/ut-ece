import java.net.*;
import java.io.*;
import java.util.*;

public class TcpThread implements Runnable {
	
    private Socket socket;
    private String response;
    private String cmd;
    private ObjectOutputStream output;
    private ObjectInputStream input;


    public TcpThread(Socket socket) {
        this.socket = socket;
        try {
            this.output = new ObjectOutputStream(socket.getOutputStream());
            this.input = new ObjectInputStream(socket.getInputStream());
            
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        try {
            while ((cmd = (String) input.readObject()) != null) {
                // Perform task
          		String[] tokens = cmd.split(" ");
          		if (tokens[0].equals("purchase")) {
                    String userName = tokens[1];
                    String productName = tokens[2];
                    Integer quantity = Integer.parseInt(tokens[3]);
                    response = Server.store.purchase(userName, productName, quantity);
          		} else if (tokens[0].equals("cancel")) {
                    Integer orderId = Integer.valueOf(tokens[1]);
                    response = Server.store.cancel(orderId);
          		} else if (tokens[0].equals("search")) {
                    String userName = tokens[1];
                    response = Server.store.search(userName);
          		} else if (tokens[0].equals("list")) {
                    response = Server.store.list();
          		}
                synchronized(socket){
                    output.writeObject(response);
                    output.flush();
                }
              }
              socket.close();
        } catch (Exception e) {}
      }
}
