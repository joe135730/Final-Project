package Algorithm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;

import People.PeopleManager;
import People.Person;

public class Test {
  private Map<Person, List<Edge>> adjacencyList = new HashMap<>();
  private Map<Person, Person> match; // Maps each person to their match in the graph
  private Map<Person, Boolean> visited; // Tracks visited persons during search
  private Map<Person, Integer> dist; // Distance for BFS

  // Constructor for creating a bipartite graph
  public Test(PeopleManager pm) {
    initializeGraph(pm);
  }

  private void initializeGraph(PeopleManager pm) {
    List<Person> males = pm.getMaleList();
    List<Person> females = pm.getFemaleList();
    for (Person male : males) {
      adjacencyList.put(male, new ArrayList<>());
      for (Person female : females) {
        int weight = calculateWeight(male, female);
//        System.out.println("Weight between " + male.getName() + " and " + female.getName() + ": " + weight); // Debugging line
        if (weight >= 50) { // Add an edge if weight > 50
          adjacencyList.get(male).add(new Edge(female, weight));
//          System.out.println("Edge created between " + male.getName() + " and " + female.getName()); // Debugging line
        }
      }
    }
  }

  private int calculateWeight(Person user, Person prefer) {
    int weight = 0;
    if (user.getPreferCandidates().contains(prefer)) weight += 25; // 25% for being in prefer list
    if (prefer.getPreferCandidates().contains(user)) weight += 25; // Another 25% if mutual
    if (user.getInterest().equals(prefer.getInterest())) weight += 20; // 20% for shared interest
    if (user.getReligion().equals(prefer.getReligion())) weight += 10; // 10% for same religion
    // Distance of two People
    double distance = Math.sqrt(
            Math.pow(Math.abs(user.getCoordinates()[0] - prefer.getCoordinates()[0]), 2) +
                    Math.pow(Math.abs(user.getCoordinates()[1] - prefer.getCoordinates()[1]), 2));
//    System.out.println("Distance is : " + distance); // Debugging line
    if (distance <= 100) weight += 20; // 20% for close coordinates
    return weight;
  }

  public Map<Person, List<Edge>> getAdjacencyList() {
    return adjacencyList;
  }

  // SAP approach
  private boolean bfs() {
    Queue<Person> queue = new LinkedList<>();
    for (Person person : adjacencyList.keySet()) {
      if (!match.containsKey(person)) { // Person is unmatched
        dist.put(person, 0);
        queue.add(person);
      } else {
        dist.put(person, Integer.MAX_VALUE);
      }
    }

    boolean foundAugmentingPath = false;
    while (!queue.isEmpty()) {
      Person person = queue.poll();
      if (dist.get(person) < Integer.MAX_VALUE) {
        for (Edge edge : adjacencyList.get(person)) {
          Person candidate = edge.getTarget();
          // Check the match and ensure we only consider the next level
          if (!dist.containsKey(candidate) || dist.get(candidate) == Integer.MAX_VALUE) {
            dist.put(candidate, dist.get(person) + 1);
            if (!match.containsKey(candidate)) {
              foundAugmentingPath = true; // Found an augmenting path to an unmatched person
            } else {
              // Next level for BFS
              Person next = match.get(candidate);
              dist.put(next, dist.get(candidate) + 1);
              queue.add(next);
            }
          }
        }
      }
    }
    return foundAugmentingPath;
  }

  // SAP approach
  private boolean dfs(Person person) {
    if (visited.getOrDefault(person, false)) {
      return false;
    }
    visited.put(person, true);

    for (Edge edge : adjacencyList.get(person)) {
      Person candidate = edge.getTarget();
      if (dist.get(candidate) == dist.get(person) + 1) { // Ensure it follows SAP level structure
        if (!match.containsKey(candidate) || (match.get(candidate) != person && dfs(match.get(candidate)))) {
          if (!person.getPotentialCandidates().contains(candidate)) {
            match.put(person, candidate);
            match.put(candidate, person);
            person.addPotentialCandidate(candidate);
            candidate.addPotentialCandidate(person); // Ensure mutual adding
          }
          // Continue DFS to allow multiple potential matches
        }
      }
    }
    return !person.getPotentialCandidates().isEmpty(); // Check if any potential candidates were added
  }

  //  Bipartite matching via Shortest Augmenting Path (SAP) Algorithm
  public void executeSAP() {
    match = new HashMap<>();
    visited = new HashMap<>();

    dist = new HashMap<>();
    while (bfs()) { // While there is an augmenting path
      visited.clear(); // Clear visited for the next DFS
      for (Person person : adjacencyList.keySet()) {
        if (!match.containsKey(person)) {
          dfs(person);
        }
      }
    }

  }
}
