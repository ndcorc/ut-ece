/*
 *
 *	Nolan Corcoran
 *	ndc466
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

class Edge extends ShapeComponent {

	static final int BASIC = 1;
	static final int BRANCH = 2;
	static final int REJECTED = 3;

    Node a, b;
    int edgeID;
    int inTransit;
    boolean isBranch;
    int state;

    Edge(Node a, Node b) {
      	super(new Line2D.Double(a.getCenter(),b.getCenter()), Color.orange);
      	edgeID = Server.edgeCount++;
      	GraphPanel.graphPane.add(this);

      	this.a = a;
      	this.b = b;
      	a.getNeighbors().put(b, this);
      	b.getNeighbors().put(a, this);
      	this.state = BASIC;
    }

    int getCost() { // use Euclidian distance
      return ((int) a.getCenter().distance(b.getCenter()))*1000 + edgeID;
    }

    void forwardMessage(Node src, Node dest, Message message) {
      	if (!((src == a && dest == b) || (src == b && dest == a))) {
			throw new IllegalArgumentException("Wrong path");
		}

      	if (message instanceof Reject) {
 			if (!isBranch) { 
 				this.setForeground(Color.red); 
 			}
      	}
      	else if (message instanceof Connect) {
    		this.setForeground(Color.black);
    		this.repaint();
      	}
      	else if (message instanceof Initiate) {		
			isBranch = true;
			this.setForeground(Color.green); 
			this.repaint();
			dest.level = message.level;
			dest.core = message.core;
			dest.updateLabel();
      	}
      	(new Thread() {
	  		public void run() { // animation from src to dest, then delivery
			    if (GraphPanel.getDelay() > 0) {
		      		try { Thread.sleep(GraphPanel.getDelay()*20*src.priorCount); } 
		      		catch (InterruptedException ie) {
		      			ie.printStackTrace();
		      		}
		      		JLabel msgView = new JLabel(message.info);
		      		msgView.setForeground(message.color);
		      		Point2D srcPos = src.getCenter();					// JButton
		      		Point2D destPos = dest.getCenter();					// JButton
		      		msgView.setSize(msgView.getPreferredSize());
		      		msgView.setLocation((int) srcPos.getX(), (int) srcPos.getY());
		      		GraphPanel.graphPane.add(msgView);
		      		int steps = 50;
		      		int delay = 20;
		      		double deltaX = ((double) destPos.getX() - srcPos.getX()) / steps;
		      		double deltaY = ((double) destPos.getY() - srcPos.getY()) / steps;
		      		for (int i = 0; i < steps; i++) {
						msgView.setLocation((int) (srcPos.getX() + deltaX*i),
					    (int) (srcPos.getY() + deltaY*i));
						waitIfPaused();
						try { Thread.sleep(GraphPanel.getDelay()); } 
						catch (InterruptedException ie) {
							ie.printStackTrace();
						}
		      		}
			      	GraphPanel.graphPane.remove(msgView);
			    }
			    try {
				    dest.sendMessage(message);
				} catch (Exception e) {
					e.printStackTrace();
				}
	  		}
		}).start();
    }

    void waitIfPaused() {
    	if (GraphPanel.pauseButton.isSelected()) {
			synchronized (GraphPanel.pauseButton) {
	 			while (GraphPanel.pauseButton.isSelected()) {
	    			try {
	      				GraphPanel.pauseButton.wait();
	    			} catch (InterruptedException ie) {
	    				ie.printStackTrace();
	    			}
	    		}
			}
		}
    }

    public String toString() {
      return "edge (" + a.UID + "," + b.UID + ")";
    }
}