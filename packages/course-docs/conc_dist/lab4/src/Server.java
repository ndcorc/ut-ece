import java.net.*;
import java.io.*;
import java.util.*;

public class Server {

    public static final Boolean DEBUG = true;

    public static Store store;
    public static Lamport lamport;
    public static ServerSocket listener;

    public static void main (String[] args) {

        Settings settings = new Settings(args);

        if (DEBUG) settings.initServerFiles();
        else settings.initServer();

        store = new Store(settings.getInventory());

        try { listener = new ServerSocket(settings.getMyServer().getPort()); } 
        catch (IOException e) {}
               
        Linker linker = new Linker(settings);
        lamport = new Lamport(linker);
        new Thread(linker).start();
        try {
            Socket socket;
            while ((socket = listener.accept()) != null) {
                new Thread(new ClientHandler(socket, lamport)).start();
                System.out.println("...");
            }
        } catch (IOException e) {
            System.out.println("Server crashed ...");
        }
    }
}