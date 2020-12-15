import java.io.PrintStream;
import java.util.Scanner;

public class TcpClient {
	
	private PrintStream pout;
	private Scanner din;
	
	public TcpClient(PrintStream pout, Scanner din) {
		this.pout = pout;
		this.din = din;
	}
	
	public void execute(String cmd){
	    pout.println(cmd);
	    pout.flush();

	    while (din.hasNextLine()) {
	      String output = din.nextLine();
	      if (output.equals("done")) {
	        break;
	      }
	      System.out.println(output);
	    }
	}
}
