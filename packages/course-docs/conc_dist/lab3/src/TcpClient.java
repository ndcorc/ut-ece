import java.net.*;
import java.io.*;
import java.util.*;

public class TcpClient {
	
	private ObjectOutputStream output;
	private ObjectInputStream input;
	private String response;
	
	public TcpClient(ObjectOutputStream output, ObjectInputStream input) {
		this.output = output;
		this.input = input;
	}
	
	public void execute(String cmd){
		try {
		    //pout.println(cmd);
		    //pout.flush();

	        output.writeObject(cmd);
	        output.flush();
	        response = (String) input.readObject();
	        System.out.println(response);
	    } catch (Exception e) {
            System.out.println("\nConnection to server has been lost ...");
            System.exit(-1);
	    }
	    /*
			    while ((output = din.nextLine()) != null) {
			      if (output.equals("done")) break;
			      System.out.println(output);
			    }
			} catch (NoSuchElementException e) {
				System.out.println("\nLost connection to the server...exiting");
				System.exit(-1);
			}
			*/

	}
}
