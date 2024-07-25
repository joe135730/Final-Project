package Algorithm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import People.PeopleManager;
import People.Person;

public class Graph {
  private Map<Person, List<Edge>> adjacencyList = new HashMap<>();

  // Constructor for creating a bipartite graph
  public Graph(PeopleManager pm) {
    List<Person> allUsers = new ArrayList<>();
    allUsers.addAll(pm.getMaleList());
    allUsers.addAll(pm.getFemaleList());

    // Initialize each person as a node in the graph
    for (Person user : allUsers) {
      adjacencyList.put(user, new ArrayList<>());
    }

    // Assuming a method to determine if two users should be connected
    for (Person user : allUsers) {
      for (Person otherUser : allUsers) {
        if (!user.equals(otherUser)) { // Ensure not to connect a user to themselves
          int weight = calculateWeight(user, otherUser);
          addEdge(user, otherUser, weight);
        }
      }
    }
  }

  private void addEdge(Person from, Person to, int weight) {
    if (!adjacencyList.containsKey(from)) {
      adjacencyList.put(from, new ArrayList<>());
    }
    if (!adjacencyList.containsKey(to)) {
      adjacencyList.put(to, new ArrayList<>());
    }

    adjacencyList.get(from).add(new Edge(to, weight));
    // For undirected graph, add the reverse edge as well
    adjacencyList.get(to).add(new Edge(from, weight));
  }

  private int calculateWeight(Person user, Person prefer) {
    int weight = 0;
    if (user.getPreferCandidates().contains(prefer)) weight += 20; // 20% for being in prefer list
    if (prefer.getPreferCandidates().contains(user)) weight += 20; // Another 20% if mutual
    if (user.getInterest().equals(prefer.getInterest())) weight += 20; // 20% for shared interest
    if (user.getReligion().equals(prefer.getReligion())) weight += 10; // 10% for same religion
    if (Math.abs(user.getCoordinates()[0] - prefer.getCoordinates()[0]) +
            Math.abs(user.getCoordinates()[1] - prefer.getCoordinates()[1]) < 50) weight += 20; // 20% for close coordinates
    weight += (user.getRating() + prefer.getRating()) * 5; // 10% from ratings
    return weight;
  }

  public Map<Person, List<Edge>> getAdjacencyList() {
    return adjacencyList;
  }
}
