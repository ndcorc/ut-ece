import java.net.*;
import java.io.*;
import java.util.*;

public class ListenerThread extends Thread {
    
    private Integer pid;
    private Linker linker;
    public Message message;

    public ListenerThread(Integer pid, Linker linker) {
        this.pid = pid;
        this.linker = linker;
        this.message = null;
    }

    public void run() {
        while (true) {
            try {
                message = (Message) linker.receiveMsg(pid);
            } catch (Exception e) {
                break;
            }
            if (message instanceof ServerRequest) {
                Server.lamport.receiveRequest((ServerRequest) message, pid);
            }
            else if (message instanceof Acknowledgement) {
                linker.lock.lock();
                linker.condition.signalAll();
                linker.lock.unlock();
                Server.lamport.receiveAck((Acknowledgement) message, pid);
            }
            else if (message instanceof Update) {
                Server.lamport.receiveUpdate((Update) message, pid);
            }
            else if (message instanceof Release) {
                Server.lamport.receiveRelease((Release) message, pid);
            }
            else {
                System.out.println("ERROR: Unknown message received");
            }
        }  
    } 
}
