import java.net.*;
import java.io.*;
import java.util.*;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.TimeUnit;


public class Lamport {

	public LamportClock c;
	public Queue<ServerRequest> q;
	public Comparator<ServerRequest> comparator;
	public int numAcks;
	public Linker linker;

	public Lamport(Linker linker) {
		this.linker = linker;
		this.c = new LamportClock();
        this.comparator = new Comparator<ServerRequest>() {
            public int compare(ServerRequest a, ServerRequest b) {
                return ServerRequest.compare(a, b);
            }
        };
        q = new PriorityQueue<ServerRequest>(linker.N+1, comparator);
        numAcks = 0;
	}

	public synchronized void requestCS(ClientRequest request) {

		c.tick();
		ServerRequest message = new ServerRequest(c.getValue(), linker.getMyId(), request.message);
		q.add(message); 																// ADD REQUEST TO QUEUE
		numAcks = 0;
        Set<Integer> keySet = linker.connector.connections.keySet();
        Iterator<Integer> it = keySet.iterator();
        while (it.hasNext()) {
        	Integer pid = it.next();
			try {
				linker.sendMsg(pid, message);
				linker.lock.lock();
                Boolean serverCrashed = !linker.condition.await(100, TimeUnit.MILLISECONDS);
                linker.lock.unlock();
                if (serverCrashed) {
                    System.out.println("Server " + pid + " crashed");
                    it.remove();
                    linker.N--;
                }
	        } catch (Exception e) {
	        	e.printStackTrace();
	        }
		}
        while((q.peek().pid != linker.getMyId()) || (numAcks < linker.N)) {
            try {
                wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
	}

    public synchronized ServerResponse execute(ClientRequest request) {
        String[] tokens = request.message.split(" ");
        ServerResponse response = null;
        switch(tokens[0]) {
        	case "purchase":
	            String username = tokens[1];
	            String product = tokens[2];
	            String quantity = tokens[3];		
	            response = new ServerResponse(Server.store.purchase(username, product, quantity));
	            if (response.message.split(" ")[0] != "Not") {	// i.e. purchase made
	            	response.updateNeeded = "Purchase";
	            }
	            break;
	        case "cancel":
	            response = new ServerResponse(Server.store.cancel(Integer.valueOf(tokens[1])));
	            if (response.message.split(" ")[0] != "Not") {	// i.e. order canceled
	            	response.updateNeeded = "Cancel";
	            }
	            break;          
	        case "search":
	            response = new ServerResponse(Server.store.search(tokens[1]));
	            break;
	        case "list":
	            response = new ServerResponse(Server.store.list());
	            break;
	       	default:
	    		System.out.println("ERROR: No such command");
	    		break;
	    }
	    return response;
    }

    public synchronized void updateServers(ServerResponse response) {
    	Message message = new Update(response.message);

        Set<Integer> keySet = linker.connector.connections.keySet();
        Iterator<Integer> it = keySet.iterator();
        while (it.hasNext()) {
        	Integer pid = it.next();
			try {
				linker.sendMsg(pid, message);
	        } catch (Exception e) {
	        	e.printStackTrace();
	        }
		}
    }

	public synchronized void releaseCS(ClientRequest request) {
		q.poll();
		Message message = new Release(request.message, c.getValue());
        Set<Integer> keySet = linker.connector.connections.keySet();
        Iterator<Integer> it = keySet.iterator();
        while (it.hasNext()) {
        	Integer pid = it.next();
			try {
				linker.sendMsg(pid, message);
	        } catch (Exception e) {
	        	e.printStackTrace();
	        }
		}
	}
	
	public synchronized void receiveRequest(ServerRequest request, Integer src) {	// SEND ACKKNOWLEDGEMENT MESSAGE
		q.add(request);																// ADD REQUEST TO QUEUE
		c.receiveAction(request.logicalClock);										// UPDATE CLOCK
		Message message = new Acknowledgement(request.message, c.getValue());
		try {
			linker.sendMsg(request.pid, message);
		} catch (Exception e) {}
		notifyAll();
	}

	public synchronized void receiveAck(Acknowledgement ack, Integer src) {
		c.receiveAction(ack.logicalClock);
        numAcks += 1;
        notifyAll();
	}
	
	/* UPDATE STORE INSTANCE THEN SEND RELEASE MESSAGE */
	public synchronized void receiveRelease(Release release, Integer src) {

		c.receiveAction(release.logicalClock);
        for (ServerRequest request : q) {	
        	if (request.pid.equals(src)) {
        		q.remove(request);
        	}
        }

        notifyAll();
    }

    public synchronized void receiveUpdate(Update update, Integer src) {
    	Server.store.updateInventory(update.inventory);
    	Server.store.updateOrders(update.orders);
    	Server.store.updateCustomers(update.customers);
    	Server.store.updateOrderIds(update.orderIds);
    	System.out.println("Store has been updated by server " + Integer.toString(src));
    }

}
