import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class UdpClient {
    
	private InetAddress ia;
	private int bufferLen;
    private DatagramSocket datasocket;
    private DatagramPacket sPacket, rPacket ;
    private int port;
    private byte[] rbuffer;
    
    public UdpClient(InetAddress ia, DatagramSocket datasocket, int port) {
    	this.ia = ia;
    	this.bufferLen = 1024;
        this.datasocket = datasocket;
        this.port = port; 
        rbuffer = new byte[bufferLen];
    }
	public void execute(String cmd){
	    try {
			byte[] buffer = new byte[cmd.length()];
			buffer = cmd.getBytes();		
			sPacket = new DatagramPacket(buffer, buffer.length, ia, port);
			datasocket.send(sPacket);
			rPacket = new DatagramPacket(rbuffer, rbuffer.length);
			datasocket.receive(rPacket);
			String retstring = new String(rPacket.getData(), 0, rPacket.getLength());
			System.out.println(retstring);
		} catch (IOException e) {
			System.out.println("\nLost connection to the server...exiting");
			System.exit(-1);
	    }
	}
}
