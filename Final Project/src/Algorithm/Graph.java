package Algorithm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import People.PeopleManager;
import People.Person;

public class Graph {
  private Map<Person, List<Edge>> adjacencyList = new HashMap<>();
  private Map<Person, Person> match; // Maps each person to their match in the graph
  private Map<Person, Boolean> visited; // Tracks visited persons during search

  // Constructor for creating a bipartite graph
  public Graph(PeopleManager pm) {
    initializeGraph(pm);
  }

  private void initializeGraph(PeopleManager pm) {
    List<Person> males = pm.getMaleList();
    List<Person> females = pm.getFemaleList();
    for (Person male : males) {
      adjacencyList.put(male, new ArrayList<>());
      for (Person female : females) {
        int weight = calculateWeight(male, female);
        if (weight > 0) { // Add an edge if there's a positive weight
          adjacencyList.get(male).add(new Edge(female, weight));
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
    if (Math.abs(user.getCoordinates()[0] - prefer.getCoordinates()[0]) +
            Math.abs(user.getCoordinates()[1] - prefer.getCoordinates()[1]) < 50) weight += 20; // 20% for close coordinates
    return weight;
  }

  public Map<Person, List<Edge>> getAdjacencyList() {
    return adjacencyList;
  }

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
      if (!match.containsKey(candidate) || dfs(match.get(candidate))) {
        match.put(person, candidate);
        match.put(candidate, person);
        person.addPotentialCandidate(candidate);
        if (person.getPotentialCandidates().size() < person.getRating()) {
          return true;
        }
      }
    }
    return person.getPotentialCandidates().size() == person.getRating();
  }

  public void executeSAP() {
    match = new HashMap<>();
    visited = new HashMap<>();

    for (Person person : adjacencyList.keySet()) {
      if (!match.containsKey(person)) {
        visited.clear();
        dfs(person);
      }
    }
  }


}
