import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.regex.PatternSyntaxException;

public class Store {
	
	BufferedReader input;
	
	private HashMap<String,Integer> inventory = new HashMap<String,Integer>();
	private HashMap<Integer,ArrayList<String>> orders = new HashMap<Integer,ArrayList<String>>();
	private HashMap<String,ArrayList<Integer>> customers = new HashMap<String,ArrayList<Integer>>();
	private ArrayList<Integer> orderIds = new ArrayList<Integer>();
	
	Store(String fileName) {
		for (int i = 1000; i < 2000; i++) {
			orderIds.add(i);
		}
		String line = null;
		  try {
			  input = new BufferedReader(new FileReader(fileName));
			  while ((line = input.readLine()) != null) {
				  String[] product = line.split(" ");
				  inventory.put(product[0], Integer.parseInt(product[1]));
			  }
		  } catch (IOException e) {
			  e.printStackTrace();
		  }	
	}
	
	public Integer getQuantity(String name) {
		return inventory.get(name);
	}
	
	public synchronized String purchase(String userName, String productName, String quantity) {
		String output;
		Integer orderId;
		if (inventory.containsKey(productName)) {
			if (Integer.parseInt(quantity) > inventory.get(productName)) {
				output = "Not available - Not enough items";
			} else {
				Collections.shuffle(orderIds);
				orderId = orderIds.get(0);
				ArrayList<String> order = new ArrayList<String>();	// order information
				order.add(userName);
				order.add(productName);
				order.add(quantity);
				orders.put(orderId, order);
				if (customers.containsKey(userName)) {		// customer already in the store database
					customers.get(userName).add(orderId);
				} else {
					ArrayList<Integer> customerOrders = new ArrayList<Integer>();
					customerOrders.add(orderId);
					customers.put(userName, customerOrders);
				}
				inventory.put(productName, inventory.get(productName)-Integer.parseInt(quantity));
				output = "Your order has been placed, "
				+ Integer.toString(orderId) + " "
				+ userName + " " + productName 
				+ " " + quantity;
			}
		} else {
			output = "Not Available - We do not sell this product";
		}
		return output + '\n';
	}
	
	public synchronized String cancel(Integer order) {
		ArrayList<String> orderInfo;
		String output;
		if ((orderInfo = orders.get(order)) != null) {	
			String userName = orderInfo.get(0);
			String productName = orderInfo.get(1);
			Integer quantity = Integer.valueOf(orderInfo.get(2));
			ArrayList<Integer> userOrders = customers.get(userName);
			userOrders.remove(order);
			customers.replace(userName, userOrders);
			orders.remove(order);
			inventory.replace(productName, inventory.get(productName)+quantity);
			output = "Order " + Integer.toString(order) + " is canceled";
		} else {
			output = Integer.toString(order) + " not found, no such order"; 
		}
		return output + '\n';
	}
	
	public synchronized String search(String name) {
		ArrayList<Integer> userOrders = customers.get(name);
		String output = "";
		if (userOrders != null) {
			for (Integer order : userOrders) {
				output += order + ", " + orders.get(order).get(1) + ", " + orders.get(order).get(2) + "\n";
			}
		} else {
			output = "No order found for " + name;
		}
		return output;
	}
	
	public synchronized String list() {
		String output = "";
		for (String product : inventory.keySet()) {
			output += product + " " + inventory.get(product) + "\n";
		}
		return output;
	}
	public synchronized HashMap<String,Integer> getInventory() {
		return this.inventory;
	}
	public synchronized HashMap<Integer,ArrayList<String>> getOrders() {
		return this.orders;
	}
	public synchronized HashMap<String,ArrayList<Integer>> getCustomers() {
		return this.customers;
	}
	public synchronized ArrayList<Integer> getOrderIds() {
		return this.orderIds;
	}
	public synchronized void updateInventory(HashMap<String,Integer> inventory) {
		this.inventory = inventory;
	}
	public synchronized void updateOrders(HashMap<Integer,ArrayList<String>> orders) {
		this.orders = orders;
	}
	public synchronized void updateCustomers(HashMap<String,ArrayList<Integer>> customers) {
		this.customers = customers;
	}
	public synchronized void updateOrderIds(ArrayList<Integer> orderIds) {
		this.orderIds = orderIds;
	}

}
