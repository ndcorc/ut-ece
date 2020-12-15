/*
 *
 *  Nolan Corcoran
 *  ndc466
 *
 */

import java.io.*;
import java.net.*;
import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;

public class Server {
    //communication:
    public static final Integer PORT_NUM = 8080;
    public static ServerSocket listener;

    //data:
    static final Integer MAX_NODES = 50; // limit on the number of nodes in graph
    static Boolean started; // true when the algorithm has been started up by user
    static ArrayList<Node> nodes = new ArrayList<Node>();   // Node objects indexed by unique IDs
    static ArrayList<Edge> edges = new ArrayList<Edge>();
    static Integer edgeCount = 0;
    static Integer degree = 2; // incident edges per node (must be EVEN and less than number of nodes)
    
    static final Color TRANSLUCENT_GRAY = new Color(50,50,50,128);
    static final Integer MAX_DELAY = 50;
    static final Integer WIDTH = 800;
    static final Integer HEIGHT = 600;
    static final Integer COLUMNS = 35;
    static final Integer ROWS = 50;

    static final int[][] testCases = new int[][]{
        {39, 6},
        {6, 9},
        {23, 13},
        {27, 30},
        {12, 24}
    };

    static boolean[][] positionsInUse;

    synchronized static void setStarted(Boolean b) {started = b;}
    synchronized static Boolean isStarted() { return started; }

    static void reset() {
        GraphPanel.graphPane.removeAll();
        positionsInUse = new boolean[ROWS][COLUMNS];
        closeConnections();
        edgeCount = 0;
        nodes = new ArrayList<Node>();
        nodes.add(null);  // entry 0 is unused since UID=0 is this server
        setStarted(false);
        GraphPanel.startButton.setEnabled(true);
    }

    static void closeConnections() {
        Iterator<Node> it = nodes.iterator();
        while (it.hasNext()) {
            Node n = it.next();
            if (n != null) {
                n.closeConnection("normal termination");
            }
        }
    }

    static void acceptRegistration() {
        System.out.println("Accepting registration ...");
        // throw exception when started
        while ((nodes.size() < MAX_NODES+1 || nodes.size() <= 3) && !isStarted()) {
        	try { 
                listener.setSoTimeout(5000);
                nodes.add(new Node(listener.accept(), nodes.size())); 
            }
            catch (Exception e) {}
        }
    }

    static void createEdges() {
        System.out.println("Creating edges ...");
        degree = GraphPanel.getDegreeFromUser();
        // first, create a cycle through the graph to ensure connectedness
        System.out.println("Degree = " + degree);
        for (Integer i = 1; i < nodes.size()-1; i++) {
            edges.add(new Edge(nodes.get(i), nodes.get(i+1)));
        }
        edges.add(new Edge(nodes.get(1), nodes.get(nodes.size()-1)));
        // At this point, all nodes have degree 2.
        // Now add random edges to bring each node up to the proper degree.
        Integer edgeLimit = Math.min(degree, nodes.size()-1);
        for (Integer currentLimit = 4; currentLimit <= edgeLimit; currentLimit+=2) {
            Iterator<Node> it = nodes.iterator();
            while (it.hasNext()) {
                Node node = it.next();
            	if (node != null) {
                    while (node.getNeighbors().size() < currentLimit) {
                	    int candidate = (int) (Math.random() * (nodes.size()-1)) + 1;
                	    Node dest = nodes.get(candidate); 
                	    if (dest != node && dest.getNeighbors().size() < currentLimit+1 && !dest.getNeighbors().containsKey(node)) {
                	       edges.add(new Edge(node,dest));
                	    }
                    }
            	}
            }
        }
        
    }

    static void startGHS() {
        System.out.println("Sending edges ...");
        Iterator<Node> it = nodes.iterator();
        while (it.hasNext()) {
            Node node = it.next();
            if (node != null) node.sendEdges();
        }
    }

    public static void main(String[] args) {
        GraphPanel.init(WIDTH, HEIGHT, COLUMNS, ROWS);

        System.out.println("Starting main ...");
        while (true) {
            try {
                listener = new ServerSocket(8080);    
                reset();
                acceptRegistration();
                createEdges();
                startGHS();
                GraphPanel.waitForTermination();
            } catch (Exception e) { 
                System.out.println("Server reset due to: ");
                e.printStackTrace();
            } 
            finally {
                try {
                    listener.close();
                } catch (Exception e) {}
            }
        }
    }
}

