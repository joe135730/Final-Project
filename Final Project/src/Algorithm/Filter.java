package Algorithm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import People.EducationLevel;
import People.Person;


public class Filter implements IFilter {

  /**
   * An indexCandidates method that combines interest attributes into a key.
   * Increases speed in finding potential matches based on specified interests.
   *
   * @param candidates List of candidates to index
   * @return map where each key is a combination of interestEthnicity, interestEducation, and relationshipGoal,
   * mapping to a list of candidates who share these attributes.
   */
  private Map<String, List<Person>> indexCandidates(List<Person> candidates) {
    Map<String, List<Person>> indexMap = new HashMap<>();
    for (Person candidate : candidates) {
      String key = createIndexKey(candidate);
      indexMap.computeIfAbsent(key, k -> new ArrayList<>()).add(candidate);
    }
    return indexMap;
  }

  /**
   * Constructs a key for indexing based on a person's interest in ethnicity, education, and relationship goal.
   * This key helps in categorizing candidates for faster search during the matching process.
   *
   * @param person The person whose interests are used to form the key.
   * @return A string key representing the combination of interest attributes.
   */
  private String createIndexKey(Person person) {
    return person.getInterestEthnicity() + "-" +
            person.getInterestEducation() + "-" +
            person.getRelationshipGoal();
  }


  /**
   * Checks if one person meets the interest criteria of another person.
   * This includes matching based on age range, mutual ethnicity interest, education level, and relationship goals.
   *
   * @param candidate        The person being considered as a potential match.
   * @param interestedPerson The person whose preferences are being checked against the candidate.
   * @return True if all conditions are met, false otherwise.
   */
  private boolean isQualifiedCandidate(Person candidate, Person interestedPerson) {
    boolean ageMatch = candidate.getAge() >= interestedPerson.getInterestAge()[0] &&
            candidate.getAge() <= interestedPerson.getInterestAge()[1];
    boolean ethnicityMatch = candidate.getEthnicity().equalsIgnoreCase(interestedPerson.getInterestEthnicity()) &&
            interestedPerson.getEthnicity().equalsIgnoreCase(candidate.getInterestEthnicity());
    EducationLevel candidateEducation = EducationLevel.fromString(candidate.getEducation());
    EducationLevel requiredEducation = EducationLevel.fromString(interestedPerson.getInterestEducation());
    boolean educationMatch = candidateEducation != null && requiredEducation != null &&
            candidateEducation.getRank() >= requiredEducation.getRank();
    boolean relationshipGoalMatch = candidate.getRelationshipGoal().equalsIgnoreCase(interestedPerson.getRelationshipGoal());

    return ageMatch && ethnicityMatch && educationMatch && relationshipGoalMatch;
  }

  /**
   * Determines if there is a mutual match between two persons, checking if each person meets the other's criteria.
   *
   * @param person1 First person in the potential match.
   * @param person2 Second person in the potential match.
   * @return True if both persons qualify each other as matches based on their individual criteria.
   */
  private boolean mutualMatch(Person person1, Person person2) {
    return isQualifiedCandidate(person1, person2) && isQualifiedCandidate(person2, person1);
  }

  /**
   * Processes a list of male candidates and finds matches based on the preferences of females.
   *
   * @param males   List of male candidates.
   * @param females List of female candidates whose interests are used to index and match with males.
   */
  @Override
  public void addPreferCandidatesMale(List<Person> males, List<Person> females) {
    Map<String, List<Person>> femaleIndex = indexCandidates(females);

    for (Person male : males) {
      List<Person> matches = new ArrayList<>();
      String key = createIndexKey(male);
      List<Person> potentialFemales = femaleIndex.getOrDefault(key, new ArrayList<>());

      for (Person female : potentialFemales) {
        if (mutualMatch(male, female)) {
          matches.add(female);
        }
      }
      male.setPreferCandidates(matches);
    }
  }

  /**
   * Processes a list of female candidates and finds matches based on the preferences of males.
   *
   * @param males   List of male candidates whose interests are used to index and match with females.
   * @param females List of female candidates.
   */
  @Override
  public void addPreferCandidatesFemale(List<Person> males, List<Person> females) {
    Map<String, List<Person>> maleIndex = indexCandidates(males);

    for (Person female : females) {
      List<Person> matches = new ArrayList<>();
      String key = createIndexKey(female);
      List<Person> potentialMales = maleIndex.getOrDefault(key, new ArrayList<>());

      for (Person male : potentialMales) {
        if (mutualMatch(female, male)) {
          matches.add(male);
        }
      }
      female.setPreferCandidates(matches);
    }
  }
}

