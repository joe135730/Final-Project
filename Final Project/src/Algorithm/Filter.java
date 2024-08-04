package Algorithm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import People.EducationLevel;
import People.Person;


public class Filter implements IFilter{

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
      String key = candidate.getInterestEthnicity() + "-" + candidate.getInterestEducation()
              + "-" + candidate.getRelationshipGoal();
      indexMap.computeIfAbsent(key, k -> new ArrayList<>()).add(candidate);
    }
    return indexMap;
  }

  @Override
  public void addPreferCandidatesMale(List<Person> males, List<Person> females) {
    Map<String, List<Person>> femaleIndex = indexCandidates(females);

    for (Person male : males) {
      List<Person> matches = new ArrayList<>();
      String key = male.getInterestEthnicity() + "-" + male.getInterestEducation() + "-" + male.getRelationshipGoal();
      List<Person> potentialFemales = femaleIndex.getOrDefault(key, new ArrayList<>());

      for (Person female : potentialFemales) {
        if (female.getAge() >= male.getInterestAge()[0] && female.getAge() <= male.getInterestAge()[1]) {
          matches.add(female);
        }
      }
      male.setPreferCandidates(matches);
    }
  }


  @Override
  public void addPreferCandidatesFemale(List<Person> males, List<Person> females) {
    Map<String, List<Person>> maleIndex = indexCandidates(males);

    for (Person female : females) {
      List<Person> matches = new ArrayList<>();
      String key = female.getInterestEthnicity() + "-" + female.getInterestEducation() + "-" + female.getRelationshipGoal();
      List<Person> potentialMales = maleIndex.getOrDefault(key, new ArrayList<>());

      for (Person male : potentialMales) {
        if (male.getAge() >= female.getInterestAge()[0] && male.getAge() <= female.getInterestAge()[1]) {
          matches.add(male);
        }
      }
      female.setPreferCandidates(matches);
    }
  }
  
  // @Override
  // public void addPreferCandidatesMale(List<Person> males, List<Person> females) {
  //   for (Person male : males) {
  //     List<Person> matches = new ArrayList<>();
  //     for (Person female : females) {
  //       if (female.getAge() >= male.getInterestAge()[0] && female.getAge() <= male.getInterestAge()[1]
  //               && female.getEthnicity().equals(male.getInterestEthnicity())
  //               && EducationLevel.fromString(female.getEducation()).ordinal() >= EducationLevel.fromString(male.getInterestEducation()).getRank()
  //               && female.getRelationshipGoal().equals(male.getRelationshipGoal())) {
  //         // Add potential person to potentialCandidates list only if match filter
  //         matches.add(female);
  //       }
  //     }
  //     male.setPreferCandidates(matches);
  //   }
  // }

  // @Override
  // public void addPreferCandidatesFemale(List<Person> males, List<Person> females) {
  //   for (Person female : females) {
  //     List<Person> matches = new ArrayList<>();
  //     for (Person male : males) {
  //       if (male.getAge() >= female.getInterestAge()[0] && male.getAge() <= female.getInterestAge()[1]
  //               && male.getEthnicity().equals(female.getInterestEthnicity())
  //               && EducationLevel.fromString(male.getEducation()).getRank() >= EducationLevel.fromString(female.getInterestEducation()).getRank()
  //               && male.getRelationshipGoal().equals(female.getRelationshipGoal())) {
  //         matches.add(male);
  //       }
  //     }
  //     female.setPreferCandidates(matches);
  //   }
  // }

}
