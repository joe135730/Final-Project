package Algorithm;

import People.Person;

/**
 * The type Edge.
 */
public class Edge {
  /**
   * to is the vertex to be connected, which is another Person
   */
  Person to;
  /**
   * The Weight.
   */
  int weight;  // This could represent combined scores based on interests, religion, etc.
  /**
   * The Capacity.
   */
  int capacity;  // Capacity to handle multiple matches based on the person's rating

  /**
   * Instantiates a new Edge.
   *
   * @param to       the target "to" of the edge
   * @param weight   the weight
   * @param capacity the capacity of the edge
   */
  public Edge(Person to, int weight, int capacity) {
    this.to = to;
    this.weight = weight;
    this.capacity = capacity;
  }

  /**
   * Gets to.
   *
   * @return the target of the edge
   */
  public Person getTo() {
    return to;
  }

  /**
   * Gets weight.
   *
   * @return the weight
   */
  public int getWeight() {
    return weight;
  }

  /**
   * Gets capacity.
   *
   * @return the capacity
   */
  public int getCapacity() {
    return capacity;
  }
}
