import java.net.*;
import java.io.*;
import java.util.*;

public class Server {

    public static final Boolean DEBUG = true;

    public static Store store;
    public static Lamport lamport;

    public static void main (String[] args) {

        Settings settings = new Settings(args);

        if (DEBUG) settings.initServerFiles();
        else settings.initServer();

        store = new Store(settings.getInventory());
               
        Linker linker = new Linker(settings);
        lamport = new Lamport(linker);
        linker.init();
        try {
            Socket socket;
            while ((socket = linker.connector.listener.accept()) != null) {
                System.out.println("...");
                new Thread(new ClientHandler(socket, lamport)).start();
            }
        } catch (IOException e) {
            System.out.println("Server crashed ...");
        }
    }
}