import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

public class Store {
	
	BufferedReader input;
	
	private HashMap<String,Integer> inventory;
	private HashMap<Integer,ArrayList<String>> orders;
	private HashMap<String,ArrayList<Integer>> customers;
	private ArrayList<Integer> orderIds;
	
	Store() {
		inventory = new HashMap<String,Integer>();
		orders = new HashMap<Integer,ArrayList<String>>();
		customers = new HashMap<String,ArrayList<Integer>>();	
		orderIds = new ArrayList<Integer>();	
	}
	
	public void initializeInventory(String fileName) {
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
	
	public synchronized String purchase(String userName, String productName, Integer quantity) {
		String output;
		Integer orderId;
		if (inventory.containsKey(productName)) {
			if (quantity > inventory.get(productName)) {
				output = "Not available - Not enough items";
			} else {
				Collections.shuffle(orderIds);
				orderId = orderIds.get(0);
				ArrayList<String> mapping = new ArrayList<String>();
				mapping.add(userName);
				mapping.add(productName);
				mapping.add(Integer.toString(quantity));
				orders.put(orderId, mapping);
				if (customers.containsKey(userName)){
					ArrayList<Integer> orderList = customers.get(userName);
					orderList.add(orderId);
					customers.replace(userName, orderList);
				} else {
					customers.put(userName, new ArrayList<Integer>(orderId));
				}
				inventory.replace(productName, inventory.get(productName)-quantity);
				output = "Your order has been placed, " 
				+ Integer.toString(orderId) + " "
				+ userName + " " + productName 
				+ " " + Integer.toString(quantity);
			}
		} else {
			output = "Not Available - We do not sell this product";
		}
		return output;
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
		return output;
	}
	
	public synchronized String search(String name) {
		ArrayList<Integer> userOrders;
		String output = "";
		if (((userOrders = customers.get(name)) != null) && (userOrders.size() > 0)) {
			for (Integer order : userOrders) {
				output += order + ", " + orders.get(order).get(1) 
						+ ", " + orders.get(order).get(2) + "\n";
			}
		} else {
			output = "No order found for " + name;
		}
		return output;
	}
	
	public synchronized String list() {
		String output = "";
		for (String product : inventory.keySet()) {
			output += product + " " + inventory.get(product);
		}
		return output;
	}
	

}
