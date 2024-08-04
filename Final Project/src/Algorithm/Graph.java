package Algorithm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;

import People.PeopleManager;
import People.Person;

public class Graph {
  private Map<Person, List<Edge>> adjacencyList = new HashMap<>();

  // Constructor for creating a bipartite graph
  public Graph(PeopleManager pm) {
    List<Person> allUsers = new ArrayList<>();
    allUsers.addAll(pm.getMaleList());
    allUsers.addAll(pm.getFemaleList());

    for (Person user : allUsers) {
      adjacencyList.put(user, new ArrayList<>());
    }

    for (Person user : allUsers) {
      for (Person otherUser : allUsers) {
        if (!user.equals(otherUser)) {
          int weight = calculateWeight(user, otherUser);
          // Set capacity based on user's rating
          int capacity = user.getRating();
          addEdge(user, otherUser, weight, capacity);
        }
      }
    }
  }

  private void addEdge(Person from, Person to, int weight, int capacity) {
    if (!adjacencyList.containsKey(from)) {
      adjacencyList.put(from, new ArrayList<>());
    }
    if (!adjacencyList.containsKey(to)) {
      adjacencyList.put(to, new ArrayList<>());
    }
    adjacencyList.get(from).add(new Edge(to, weight, capacity));
  }


  private int calculateWeight(Person user, Person prefer) {
    int weight = 0;
    if (user.getPreferCandidates().contains(prefer)) weight += 25; // 25% for being in prefer list
    if (prefer.getPreferCandidates().contains(user)) weight += 25; // Another 25% if mutual
    if (user.getInterest().equals(prefer.getInterest())) weight += 20; // 20% for shared interest
    if (user.getReligion().equals(prefer.getReligion())) weight += 10; // 10% for same religion
    if (Math.abs(user.getCoordinates()[0] - prefer.getCoordinates()[0]) +
            Math.abs(user.getCoordinates()[1] - prefer.getCoordinates()[1]) < 50) weight += 20; // 20% for close coordinates
    return weight;
  }

  public Map<Person, List<Edge>> getAdjacencyList() {
    return adjacencyList;
  }



}
