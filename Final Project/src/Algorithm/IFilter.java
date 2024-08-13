package Algorithm;

import java.util.List;

import People.Person;

/**
 * The IFilter interface defines methods for adding preferred candidates based on gender,
 * providing a contract for implementing classes to handle gender-specific filtering of candidates.
 */
public interface IFilter {

  /**
   * Adds male candidates from a given list of males and females to the preferred list.
   * Implementing classes defines how males are selected and added.
   *
   * @param males   List of male candidates to be filtered or processed
   * @param females List of female candidates, potentially for comparison
   */
  void addPreferCandidatesMale(List<Person> males, List<Person> females);

  /**
   * Adds female candidates from a given list of males and females to the preferred list.
   * Implementing classes should define how females are selected and added.
   *
   * @param males   List of male candidates, potentially for comparison or exclusion
   * @param females List of female candidates to be filtered or processed
   */
  void addPreferCandidatesFemale(List<Person> males, List<Person> females);
}
