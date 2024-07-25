package Algorithm;

import java.util.ArrayList;
import java.util.List;

import People.EducationLevel;
import People.Person;

public class Filter implements IFilter{
  @Override
  public void addPreferCandidatesMale(List<Person> males, List<Person> females) {
    for (Person male : males) {
      List<Person> matches = new ArrayList<>();
      for (Person female : females) {
        if (female.getAge() >= male.getInterestAge()[0] && female.getAge() <= male.getInterestAge()[1]
                && female.getEthnicity().equals(male.getInterestEthnicity())
                && EducationLevel.fromString(female.getEducation()).ordinal() >= EducationLevel.fromString(male.getInterestEducation()).getRank()
                && female.getRelationshipGoal().equals(male.getRelationshipGoal())) {
          // Add potential person to potentialCandidates list only if match filter
          matches.add(female);
        }
      }
      male.setPreferCandidates(matches);
    }
  }

  @Override
  public void addPreferCandidatesFemale(List<Person> males, List<Person> females) {
    for (Person female : females) {
      List<Person> matches = new ArrayList<>();
      for (Person male : males) {
        if (male.getAge() >= female.getInterestAge()[0] && male.getAge() <= female.getInterestAge()[1]
                && male.getEthnicity().equals(female.getInterestEthnicity())
                && EducationLevel.fromString(male.getEducation()).getRank() >= EducationLevel.fromString(female.getInterestEducation()).getRank()
                && male.getRelationshipGoal().equals(female.getRelationshipGoal())) {
          matches.add(male);
        }
      }
      female.setPreferCandidates(matches);
    }
  }

}
