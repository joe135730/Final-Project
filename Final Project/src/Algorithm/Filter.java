package Algorithm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import People.EducationLevel;
import People.Person;

/**
 * Filter class implements IFilter interface.
 * This method is to filter the group by age, ethnicity, education, and relationship goal.
 */
public class Filter implements IFilter {
  private Map<String, List<Person>> dp = new HashMap<>();

  /**
   * Add Prefer Candidates Male method
   * Attributes: input List of male and list of female from Person object
   */
  @Override
  public void addPreferCandidatesMale(List<Person> males, List<Person> females) {
    Map<String, List<Person>> femalesGroup = groupAttributes(females);

    for (Person male : males) {
      String dpKey = generate(male);

      if(!dp.containsKey(dpKey)) {
        dp.put(dpKey, findMatches(male, femalesGroup));
      }
      male.setPreferCandidates(dp.get(dpKey));
    }
  }

  /**
   * Add Prefer Candidates Female method
   * Attributes: input List of male and list of female from Person object
   */
  public void addPreferCandidatesFemale(List<Person> males, List<Person> females) {
    // Group males by age, ethnicity, education, and relationship goal
    Map<String, List<Person>> maleGroup = groupAttributes(males);

    for(Person female : females){
      String dpKey = generate(female);

      if(!dp.containsKey(dpKey)) {
        dp.put(dpKey, findMatches(female, maleGroup));
      }
      female.setPreferCandidates(dp.get(dpKey));
    }
  }

  /**
   * Group Attributes method
   * Attributes: input List of people from Person object
   * This method is to group people by their attributes.
   */
  public Map<String, List<Person>> groupAttributes(List<Person> people) {
    //  key: a string to concatenation these attributes
    //  value: list of female who share these attributes
    return people.stream().collect(Collectors.groupingBy(person ->
            person.getAge() + "-" + person.getEthnicity() + "-" +
                    EducationLevel.fromString(person.getEducation()).ordinal() + "-" +
                    person.getRelationshipGoal()
    ));
  }

  /**
   * Generate method
   * Attributes: input person from the Person object
   * This method is to generate a key based on person's preference
   */
  public String generate(Person person) {
    String interestEthnicity = person.getInterestEthnicity();

    //  Converts the person's interest in education level into an ordinal value, representing the level of education
    String interestEducationRank = String.valueOf(EducationLevel.fromString(person.getInterestEducation()).ordinal());
    String interestRelationshipGoal = person.getRelationshipGoal();

    //  extract the minimum and maximum ages that person is interested in from an array
    int minAge = person.getInterestAge()[0];
    int maxAge = person.getInterestAge()[1];

    //  Constructs the key from the map based on the current age and the male's preferences
    return minAge + "-" + maxAge + "-" + interestEthnicity + "-" + interestEducationRank + "-" + interestRelationshipGoal;
  }

  /**
   * Find Matches method
   * Attributes: input List of group from Person object, person from the Person object
   * This method is to filter the group (by age, ethnicity, education, and relationship goal)
   * to find people in other group that match the person's preferences and return the prefer matches .
   */
  public List<Person> findMatches(Person person, Map<String, List<Person>> group) {
    //  initializes an empty list to store person that match the current preferences
    List<Person> preferMatch = new ArrayList<>();
    String interestEthnicity = person.getInterestEthnicity();

    //  Converts the person's interest in education level into an ordinal value, representing the level of education
    String interestEducationRank = String.valueOf(EducationLevel.fromString(person.getInterestEducation()).ordinal());
    String interestRelationshipGoal = person.getRelationshipGoal();

    //  extract the minimum and maximum ages that person is interested in from an array
    int minAge = person.getInterestAge()[0];
    int maxAge = person.getInterestAge()[1];

    for (int age = minAge; age <= maxAge; age++) {
      //  Constructs the key from the map based on the current age and the male's preferences
      String key = age + "-" + interestEthnicity + "-" + interestEducationRank + "-" + interestRelationshipGoal;
      preferMatch.addAll(group.getOrDefault(key, new ArrayList<>()));
    }
    return preferMatch;
  }
  // final test
}
