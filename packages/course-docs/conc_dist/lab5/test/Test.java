import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

// Do not change the signature of this class
public class Test {
    public static void main(String[] args)  throws IOException{

    	BufferedReader br = new BufferedReader(new FileReader("../input/chap00"));

		String line = null;
		Sting[] words;
		line = br.readLine();
		words = br.readLine().toLowerCase().replaceAll("[^a-z0-9]", " ").split("[ \t]+");
	    for (String word : words) {
	    	System.out.println(word);
	    }
		br.close();
    }
}