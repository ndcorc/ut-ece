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

class Node extends JButton implements Runnable {

	static final Integer SLEEPING = 1;
	static final Integer FIND = 2;
	static final Integer FOUND = 3;

    Socket socket;
    Integer UID;
    String name;
    Integer state;
    Integer level;
    Integer core;

    Integer best_weight;
    Edge best_edge;

    Message message;
    LinkedList<Message> messageQueue;
    Integer priorCount = 0;

    ObjectOutputStream outputStream;
    ObjectInputStream inputStream;

    HashMap<Node, Edge> neighbors;
    Boolean terminated;

    Node(Socket socket, Integer UID) {
      	this.socket = socket;
      	this.UID = UID;
      	this.core = 0;
      	this.name = "?";
      	this.state = Node.SLEEPING;
      	this.neighbors = new HashMap<Node, Edge>();
        this.messageQueue = new LinkedList<Message>();

      	addActionListener(new ActionListener() {
  	  		public void actionPerformed(ActionEvent ae) {
  	    		sendMessage(new Wakeup(0, UID)); //	src, dest
  	  		}
		});

      	(new Thread(this)).start();
      	setOpaque(false);
      	setBorder(null);
    }

    HashMap<Node, Edge> getNeighbors() {
     	return neighbors;
    }

    void updateLabel() {
      	setText(name + " - " + UID+"("+core + "," + level + ")");
      	this.setSize(this.getPreferredSize());
    }

    public void run() {
    	System.out.println("Total nodes: " + Server.nodes.size());
      	//updateLabel();
      	setToolTipText("Node " + UID);
      	System.out.println("Total positions: " + (Server.positionsInUse.length*Server.positionsInUse[0].length));
      	this.setLocation(computeNodePosition());
      	//this.setLocation(new Point(Server.WIDTH/Server.COLUMNS*col,Server.HEIGHT/Server.ROWS*row))
        //int[] testCase = Server.testCases[UID-1];
        //this.setLocation(new Point(Server.WIDTH/Server.COLUMNS*testCase[1],Server.HEIGHT/Server.ROWS*testCase[0]));
        //this.setLocation(new Point(testCase[1],testCase[0]));
        GraphPanel.graphPane.add(this);
      	try {
			outputStream = new ObjectOutputStream(socket.getOutputStream());
			inputStream = new ObjectInputStream(socket.getInputStream());
			receiveMessages();
      	} catch (Exception e) {
            e.printStackTrace();
			closeConnection(e);
      	}
    }

  	synchronized Point computeNodePosition() {
    	boolean overlap = true;
	    int row = 0;
	    int col = 0;
        
    	while (overlap) {
      		row = (int) (Math.random()*Server.ROWS);
      		col = (int) (Math.random()*Server.COLUMNS);
      		// to avoid crowding, check the neighborhood around the node
	      	overlap = false;
	      	for (int c = Math.max(0, col-1); c <= Math.min(Server.COLUMNS-1, col+1); c++){
				for (Integer r = (Integer) Math.max(0, row-1); r <= (Integer) Math.min(Server.ROWS-1, row+1); r++) {

					if (Server.positionsInUse[r][c]) {
						overlap = true;
					}
				}
	      	}
    	}
    	Server.positionsInUse[row][col] = true;
    	return new Point(Server.WIDTH/Server.COLUMNS*col,Server.HEIGHT/Server.ROWS*row);
  	}

    synchronized void sendMessage(Message message) {
     	try {
            System.out.println("Sending " + message.getClass().getName() + " - " + message.toString());
			outputStream.writeObject(message);
			outputStream.flush();
      	} catch (Exception e) {
            e.printStackTrace();
			closeConnection(e);
      	}
    }

    void sendEdges() {
     	final TreeMap<Integer, Integer> costs = new TreeMap<Integer, Integer>();
      	Iterator<Node> it = neighbors.keySet().iterator();
      	while (it.hasNext()) {
			Node node = it.next();
			costs.put(neighbors.get(node).getCost(), node.UID); // new <Node, Edge> pair
      	}
      	(new Thread() {
	  		public void run() {
	  			System.out.println("Sending confirmation ...");
	    		sendMessage(new Confirmation(UID, costs));
	  		}
		}).start();
    }

    void receiveMessages() {
		while (true) {
			try {
				message = (Message) inputStream.readObject();
			} catch (Exception e){
                closeConnection(e);
                break;                  
            }
            if (message instanceof Registration) {
		    	name = message.name;
		    	updateLabel();
		    	setToolTipText("Node " + UID + ": " + name);          	
            }
            else if (message instanceof Initiate) {
			    updateLabel();    
			    forwardMessage(message);     	
            } 
            else {
            	forwardMessage(message);
            }
        }
    }

    synchronized void forwardMessage(Message message) {
    	//System.out.println("Forwarding message ...");
     	if (message.destId == UID) {
			sendMessage(message);
     	}
      	else {
			Node node = Server.nodes.get(message.destId);
            Edge edge = neighbors.get(node);
			edge.forwardMessage(this, node, message);
      	}
    }

    void closeConnection(Object reason) {
     	this.setEnabled(false);
      	if (!terminated) { 
			System.out.println("Closing socket to " + name + " due to: " + reason);
			if (reason instanceof Exception) ((Exception) reason).printStackTrace();
		}
      	try {socket.close();} catch (Exception se) {}
      	terminated = true;
    }

    Point2D getCenter() {	// JButton methods
     	return new Point(this.getX() + this.getWidth()/2, this.getY() + this.getHeight()/2);
    }
}