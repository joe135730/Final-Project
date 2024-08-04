package Algorithm;

import java.util.List;

import People.Person;

/**
 * The IFilter interface defines the methods that must be implemented to filter and match candidates based
 * on preferences. The interface allows different algorithms to be implemented while following the same contract,
 * which facilitates extensibility and testing.
 */
public interface IFilter {

   /**
   * Adds preferred candidates for males based on defined criteria. The method matches each male with females
   * based on their listed preferences and attributes.
   *
   * @param males   List of male persons to match.
   * @param females List of female persons to consider for matching.
   */
   void addPreferCandidatesMale(List<Person> males, List<Person> females);

  /**
   * Adds preferred candidates for females based on defined criteria. The method matches each female with males
   * based on their listed preferences and attributes.
   *
   * @param males   List of male persons to consider for matching.
   * @param females List of female persons to match.
   */
   void addPreferCandidatesFemale(List<Person> males, List<Person> females);
}
