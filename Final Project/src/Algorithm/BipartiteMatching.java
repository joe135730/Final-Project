package Algorithm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import People.PeopleManager;
import People.Person;

public class BipartiteMatching {
  private Map<Person, List<Edge>> adjacencyList = new HashMap<>();
  private Map<Person, Person> match; // Maps each person to their match in the graph
  private Map<Person, Boolean> visited; // Tracks visited persons during search
  private Map<Person, Integer> dist; // Distance for BFS

  // Constructor for creating a bipartite graph
  public BipartiteMatching(PeopleManager pm) {
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

// Dfs approach
  private boolean dfs(Person person) {
    // checks if the person has already been visited
    if (visited.getOrDefault(person, false)) {
      return false;
    }
    // Marks the current person as visited
    visited.put(person, true);

    // Iterate Potential Matches
    for (Edge edge : adjacencyList.get(person)) {
      Person candidate = edge.getTarget();
      if (!match.containsKey(candidate) || (match.get(candidate) != person && dfs(match.get(candidate)))) {
        match.put(person, candidate);
        match.put(candidate, person);
        person.addPotentialCandidate(candidate);
        candidate.addPotentialCandidate(person); // Ensure mutual adding
        return true; // Indicate a successful match
      }
    }
    return false; // No match found or possible
  }

  //  Bipartite matching via Shortest Augmenting Path (SAP) Algorithm
  public void executeSAP() {
    match = new HashMap<>();
    visited = new HashMap<>();

    boolean changed;
    do {
      changed = false;
      visited.clear();
      for (Person person : adjacencyList.keySet()) {
        if (!match.containsKey(person) || !visited.getOrDefault(person, false)) {
          visited.clear();
          if (dfs(person)) {
            changed = true; // Indicates that a new match was found
          }
        }
      }
    } while (changed); // Continue while improvements are made

  }


}
