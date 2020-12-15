import java.net.*;
import java.io.*;
import java.util.*;
import java.util.concurrent.TimeUnit;

public class ClientHandler implements Runnable {
    
    private Socket socket;

    private ObjectInputStream input;
    private ObjectOutputStream output;
    
    public ClientHandler(Socket socket) {
        this.socket = socket;
        try {
            this.input = new ObjectInputStream(socket.getInputStream());
            this.output = new ObjectOutputStream(socket.getOutputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }   
    }

    public void run() {
        while (true) {
            ClientRequest message = null;
            try {           
                message = (ClientRequest) input.readObject();
            } catch (Exception e){
                System.out.println("Client disconnected ...");
                break;                  
            }

            Server.lamport.requestCS(message);                             // SEND REQUEST MESSAGE W/ TS++
            ServerResponse response = Server.lamport.execute(message);
            try {
                synchronized(socket){
                    output.writeObject(response);
                    output.flush();
                }
            } catch (IOException e) {
                e.printStackTrace(); 
            }
            if (response.updateNeeded != null) {
                Server.lamport.updateServers(response);                    // UPDATE STORE IN OTHER SERVERS
            }
            Server.lamport.releaseCS(message);                             // UPDATE STORE INSTANCES ON OTHER SERVERS
        }
    }

}