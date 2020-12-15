/*
 *
 *	Nolan Corcoran
 *	ndc466
 *
 */

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.concurrent.ConcurrentLinkedDeque;

public class Client implements Runnable {

	public static final Integer SLEEPING = 1;
	public static final Integer FIND = 2;
	public static final Integer FOUND = 3;

	public static final Integer BASIC = 1;
	public static final Integer BRANCH = 2;
	public static final Integer REJECTED = 3;
    
    public static final Boolean DEBUG = true;
    public static Scanner sc;
    public static Boolean processing;
    public static Socket server = null;
    public static ObjectInputStream input = null;
	public static ObjectOutputStream output = null;
    public static TreeMap<Integer, Integer> orderedEdges;	// <edgeCost, destID>
    public static HashMap<Integer, Integer> edgeWeights;	// <edgeId, edgeCost>
    public static HashMap<Integer, Integer> edgeStates;		// <destId, edgeState>

    public static Message message;
    public static ConcurrentLinkedDeque<Message> messageQueue;
    public static ConcurrentLinkedDeque<Message> deferred;

    public static Integer errorCount = 0;

    public static Integer state = SLEEPING;
    public static Integer myId;
    public static Integer level;	// Fragement Level
    public static Integer core;		// ID of Fragment Core Node
    public static Integer found;
    public static Integer inBranch;
    public static Integer testNode;
    public static Integer bestNode;
    public static Integer bestWeight;

	public static Integer merged = 0;
	public static Integer absorbed = 0;
	public static Integer connected = 0;
	public static Integer initiated = 0;
	public static Integer tested = 0;
	public static Integer accepted = 0;
	public static Integer rejected = 0;
	public static Integer reported = 0;
	public static Integer changedRoot = 0;
/*
	public synchronized static Integer getCost(Integer destId) {
		for(Map.Entry<Integer, Integer> entry : orderedEdges.entrySet()) {
			System.out.println(entry.getKey() + " => " + entry.getValue());
			if (entry.getValue().equals(destId)) {
				return entry.getKey();
			}
		}
		System.out.println("Shouldn't reach here ...");
		return null;
	}
	public synchronized static void setStates(Integer destId, Integer edgeState) {
		try {
            output.writeObject(new SetStates(destId, edgeState, level, state));	// core ?
            output.flush();
        } catch (IOException e) {
        	e.printStackTrace();
        }
	}
*/
	public synchronized static void wakeup() {
		System.out.println("Waking up ...");
    	Integer destId = orderedEdges.firstEntry().getValue();	// nodeId that connects to MWOE
    	System.out.println("MWOE = edge(" + myId + ", " + destId + ")");
    	edgeStates.put(destId, BRANCH);
    	System.out.println("New edge state: BRANCH");
		level = 0;
		System.out.println("New node state: FOUND");
		state = FOUND;
		found = 0;
		core = edgeWeights.get(destId);
		connected += 1;
		try {
			System.out.println(myId + "---CONNECT--->" + destId);
            output.writeObject(new Connect(myId, destId, level, null));
            output.flush();
        } catch (IOException e) {
        	e.printStackTrace();
        }	
	}

    public synchronized static void getResponse(Message message) {
    	System.out.println("\nGetting response ...");
    	if (message instanceof Wakeup) {
    		wakeup();
    	}
    	else if (message instanceof Connect) {
    		Connect connect = (Connect) message;
    		System.out.println(myId + "<---CONNECT---" + connect.srcId);
    		if (state == SLEEPING) {
    			wakeup();
    		}
    		if (connect.edgeState != null) {
    			edgeStates.put(connect.srcId, connect.edgeState);
    		}
    		if (connect.level < level) {	// ABSORB
    			edgeStates.put(connect.srcId, BRANCH);
    			System.out.println("New edge state: BRANCH");
    			absorbed += 1;
    			initiated += 1;
    			try {
    				System.out.println(myId + "---INITIATE(a)--->" + connect.srcId);
    				output.writeObject(new Initiate(myId, connect.srcId, core, level, state));
    				output.flush();
    			} catch (IOException e) {
		        	e.printStackTrace();
		        }
		        if (state.equals(FIND)) {
		        	found += 1;
		        }
    		} 
    		else {
    			if (edgeStates.get(connect.srcId).equals(BASIC)) {
    				System.out.println("Adding Connect message to queue ...");
    				//deferred.add(connect);
    				messageQueue.addLast(connect);
    			} 
    			else {					// MERGE
    				try {
    					System.out.println(myId + "---INITIATE(b)--->" + connect.srcId);
    					output.writeObject(new Initiate(myId, connect.srcId, edgeWeights.get(connect.srcId), level+1, FIND));
    					output.flush();
    				} catch (IOException e) {
		        		e.printStackTrace();
		       		}
    			}
    		}
		} 
		else if (message instanceof Initiate) {
			//String dummy = sc.nextLine();
			Initiate initiate = (Initiate) message;
			System.out.println(myId + "<---INITIATE---" + initiate.srcId);
			System.out.print("New node state: ");
			switch (initiate.state) {
				case 1: 
					System.out.println("SLEEPING");
					break;
				case 2:
					System.out.println("FIND");
					break;
				case 3:
					System.out.println("FOUND");
					break;

			}
			if (level < initiate.level) {
				/*
				Message defer = null;
				Iterator it = deferred.iterator();
				while (it.hasNext()) {
					Message toCheck = (Message) it.next();
					if (toCheck instanceof Connect || toCheck instanceof Test) {
						defer = toCheck;					
					}
				}
				if (defer != null) {
					deferred.remove(defer);
					messageQueue.addFirst(defer);
				}
				*/
			} else {
				edgeStates.put(initiate.srcId, BRANCH);
				System.out.println("New node state: BRANCH");
			}
			level = initiate.level;
			core = initiate.core;
			state = initiate.state;
			inBranch = initiate.srcId;
			System.out.println("Node: " + myId + ", changed to level: " + level + " on fragment:" + core);
			//setStates(initiate)
			bestNode = null;
			bestWeight = Integer.MAX_VALUE;
			System.out.println("Best weight set to MAX_VALUE: " + bestWeight);
			for(Map.Entry<Integer, Integer> entry : orderedEdges.entrySet()) {
				if (!entry.getValue().equals(initiate.srcId) && edgeStates.get(entry.getValue()).equals(BRANCH)) {
					try {
						System.out.println(myId + "---INITIATE(c)--->" + entry.getValue());
    					output.writeObject(new Initiate(myId, entry.getValue(), initiate.core, initiate.level, initiate.state));
    					output.flush();						
					} catch (IOException e) {
		        		e.printStackTrace();
		       		}
		       		if (state.equals(FIND)) {
		       			found += 1;
		       		}
				}
			}
			if (initiate.state.equals(FIND)) {
				System.out.println("Executing test()");
				test();
			}
		}
		else if (message instanceof Report) {
			Report report = (Report) message;
			System.out.println(myId + "<---REPORT---" + report.srcId);
			if(report.srcId != inBranch) {
				System.out.println("in-branch");
				found -= 1;
				if (report.weight < bestWeight) {
					System.out.println("New best weight");
					bestWeight = report.weight;
					bestNode = report.srcId;
				} 
				report();
			}
			else if (state.equals(FIND)) {
    			System.out.println("Adding Report to queue ...");
    			messageQueue.addLast(report);
			}
			else if (report.weight > bestWeight) {
				System.out.println("w > bestWeight");
				changeRoot();
			}
			else if (bestWeight == Integer.MAX_VALUE && report.weight == Integer.MAX_VALUE) {
				System.out.println("HALT");
				System.exit(-1);
			}
			else {
				System.out.println("None of the the above");
			}
		}
		else if (message instanceof Test) {
			Test test = (Test) message;
			System.out.println(myId + "<---TEST---" + test.srcId);
			if (state.equals(SLEEPING)) {
				wakeup();
			}
			if(test.level > level){
				System.out.println("Adding Test message to queue ...");
				//deferred.addLast(test);
				messageQueue.addLast(test);
			}
			else if(test.core != core) {
					accepted += 1;
					try {
						System.out.println(myId + "---ACCEPT--->" + test.srcId);
    					output.writeObject(new Accept(myId, test.srcId));
    					output.flush();					
					} catch (IOException e) {
		        		e.printStackTrace();
		       		}
			}
			else {
				if (edgeStates.get(test.srcId).equals(BASIC)) {
					edgeStates.put(test.srcId, REJECTED);
					System.out.println("New edge state: REJECTED");
				}
				if (testNode != null && edgeWeights.get(testNode) != edgeWeights.get(test.srcId)) {
					rejected += 1;
					try {
						System.out.println(myId + "---REJECT--->" + test.srcId);
    					output.writeObject(new Reject(myId, test.srcId));
    					output.flush();						
					} catch (IOException e) {
		        		e.printStackTrace();
		       		}			
				}
				else {
					test();
				}
			}
		}
		else if (message instanceof Accept) {
			Accept accept = (Accept) message;
			System.out.println(myId + "<---ACCEPT---" + accept.srcId);
			testNode = null;
			Integer edgeWeight = edgeWeights.get(accept.srcId);
			System.out.println("Edge weight: " + edgeWeight);
			System.out.println("Best weight: " + bestWeight);
			if (edgeWeight < bestWeight) {
				bestNode = accept.srcId;
				bestWeight = edgeWeight;
			}
			report();
		}
		else if (message instanceof Reject) {
			Reject reject = (Reject) message;
			System.out.println(myId + "<---REJECT---" + reject.srcId);
			if (edgeStates.get(reject.srcId).equals(BASIC)) {
				edgeStates.put(reject.srcId, REJECTED);
				System.out.println("New edge state: REJECTED");
			}
			test();
		}
		else if (message instanceof ChangeRoot) {
			ChangeRoot change = (ChangeRoot) message;
			System.out.println(myId + "<---CHANGEROOT---" + change.srcId);
			changeRoot();
		}
		else {
			System.out.println("Shouldn't get here ...");
		}
    }

	public synchronized static void report() {
		if (found == 0 && testNode == null) {
			System.out.println("New state: FOUND");
			state = FOUND;
			reported += 1;
			try {
				System.out.println(myId + "---REPORT--->" + inBranch);
	            output.writeObject(new Report(myId, inBranch, bestWeight));
	            output.flush();
	        } catch (IOException e) {
	        	e.printStackTrace();
	        }
		}
	}

	public synchronized static void test() {
		testNode = null;
		for(Map.Entry<Integer, Integer> entry : orderedEdges.entrySet()) {	// should interate in ascending order
			System.out.println("Weight: " + entry.getKey());
			if (edgeStates.get(entry.getValue()).equals(BASIC)) { 
				testNode = entry.getValue();
				tested += 1;
				try {
					System.out.println(myId + "---TEST--->" + testNode);
		            output.writeObject(new Test(myId, testNode, core, level));
		            output.flush();
		        } catch (IOException e) {
		        	e.printStackTrace();
		        }		
			}
		}
		if (testNode == null) {
			report();
		}
	}

	public synchronized static void changeRoot() {
		if (edgeStates.get(bestNode).equals(BRANCH)) {
			changedRoot += 1;
			try {
				System.out.println(myId + "---CHANGEROOT--->" + bestNode);
	            output.writeObject(new ChangeRoot(myId, bestNode));
	            output.flush();
	        } catch (IOException e) {
	        	e.printStackTrace();
	        }
		}
		else {
			connected += 1;
			try {
				System.out.println(myId + "---CONNECT--->" + bestNode);
	            output.writeObject(new Connect(myId, bestNode, level, BRANCH));
	            output.flush();
	        } catch (IOException e) {
	        	e.printStackTrace();
	        }	
	        edgeStates.put(bestNode, BRANCH);
	        System.out.println("New edge state: BRANCH");	
		}
	}

    Client() {
		try { server = new Socket("127.0.0.1", 8080); }
		catch (Exception e) {
			e.printStackTrace();
            System.out.println("\nServer at 127.0.0.1:8080 has crashed");
            System.exit(-1);
        }
		sc = new Scanner(System.in);
        try {
            output = new ObjectOutputStream(server.getOutputStream());
            input = new ObjectInputStream(server.getInputStream());
        } catch (IOException e) {}
        Boolean newConnection = false;

        System.out.print("Enter your first name: ");
	    String name = sc.nextLine();
	    String[] tokens = name.split(" ");
	    while (tokens.length != 1) {
	    	System.out.print("\nInvalid, enter only one name: ");
	    	name = sc.nextLine();
	    	tokens = name.split(" ");
	    }
	    try {
            output.writeObject(new Registration(0, name));
            output.flush();
            //level = 0;

        } catch (Exception e) {
            System.out.println("\nConnection to server 127.0.0.1:8080 has been lost ...");
            System.exit(-1);
        }
        messageQueue = new ConcurrentLinkedDeque<Message>();
        deferred = new ConcurrentLinkedDeque<Message>();
    }

    public void run() {
    	//String dummy;
    	Message toHandle = null;
    	processing = false;
        while (true) {
            try {
                Thread.sleep(50);
            } 
            catch (InterruptedException ie) {}
            if (messageQueue.size() > 0) {
            	toHandle = messageQueue.poll();
				getResponse(toHandle);
			}	
        }
    }

	public static void main (String[] args) {
        Message message;
        Client client = new Client();

        new Thread(client).start();
    	while(true) {
	        try {
	        	message = (Message) input.readObject();
	        	if (message instanceof Confirmation) {
	        		myId = ((Confirmation) message).destId;
	        		orderedEdges = ((Confirmation) message).edges;
	        		edgeWeights = new HashMap<Integer, Integer>();
	        		edgeStates = new HashMap<Integer, Integer>();
					for(Map.Entry<Integer, Integer> entry : orderedEdges.entrySet()) {
						edgeWeights.put(entry.getValue(), entry.getKey());
						System.out.println("Edge: " + myId + "-->" + entry.getValue() + ", Weight: " + edgeWeights.get(entry.getValue()));
						edgeStates.put(entry.getValue(), BASIC);
						System.out.println("Edge: " + myId + "-->" + entry.getValue() + ", State: " + edgeStates.get(entry.getValue()));
					}
					System.out.println("My ID: " + myId);
	        	}
	        	else {
	        		messageQueue.add(message);
	        		System.out.println("Added message to queue, now size: " + messageQueue.size());
	        	}
	        } catch (Exception e) {
	        	e.printStackTrace();
	        	System.exit(-1);
	        }
	    }
	}
	
}