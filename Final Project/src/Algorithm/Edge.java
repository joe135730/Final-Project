package Algorithm;

import People.Person;

/**
 * The type Edge.
 */
public class Edge {
  /**
   * Target is the vertex to be connected, which is another Person
   */
  Person target;
  /**
   * The Weight.
   */
  int weight;  // This could represent combined scores based on interests, religion, etc.

  /**
   * Instantiates a new Edge.
   *
   * @param target the target
   * @param weight the weight
   */
  public Edge(Person target, int weight) {
    this.target = target;
    this.weight = weight;
  }

  /**
   * Gets target.
   *
   * @return the target
   */
  public Person getTarget() {
    return target;
  }

  /**
   * Gets weight.
   *
   * @return the weight
   */
  public int getWeight() {
    return weight;
  }
}
