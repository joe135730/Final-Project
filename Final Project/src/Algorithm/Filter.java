package Algorithm;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import People.EducationLevel;
import People.Person;

public class Filter implements IFilter {
  @Override
  public void addPreferCandidatesMale(List<Person> males, List<Person> females) {
    // Group females by age, ethnicity, education, and relationship goal
    Map<String, List<Person>> femalesGrouped = females.stream().collect(
            Collectors.groupingBy(female -> female.getAge() + "-" +
                    female.getEthnicity() + "-" +
                    EducationLevel.fromString(female.getEducation()).ordinal() + "-" +
                    female.getRelationshipGoal())
    );

    for (Person male : males) {
      List<Person> matches = new ArrayList<>();
      int minAge = male.getInterestAge()[0];
      int maxAge = male.getInterestAge()[1];
      String interestEthnicity = male.getInterestEthnicity();
      int interestEducationRank = EducationLevel.fromString(male.getInterestEducation()).ordinal();
      String interestRelationshipGoal = male.getRelationshipGoal();

      for (int age = minAge; age <= maxAge; age++) {
        String key = age + "-" + interestEthnicity + "-" + interestEducationRank + "-" + interestRelationshipGoal;
        if (femalesGrouped.containsKey(key)) {
          List<Person> femaleMatch = femalesGrouped.get(key);
          matches.addAll(femaleMatch);
        }
      }
      male.setPreferCandidates(matches);

    }
  }

  public void addPreferCandidatesFemale(List<Person> males, List<Person> females) {
    // Group males by age, ethnicity, education, and relationship goal

    Map<String, List<Person>> malesGrouped = males.stream().collect(
            Collectors.groupingBy(male -> male.getAge() + "-" +
                    male.getEthnicity() + "-" +
                    EducationLevel.fromString(male.getEducation()).ordinal() + "-" +
                    male.getRelationshipGoal())
    );

    for (Person female : females) {
      List<Person> fmatches = new ArrayList<>();
      int minAge = female.getInterestAge()[0];
      int maxAge = female.getInterestAge()[1];
      String interestEthnicity = female.getInterestEthnicity();
      int interestEducationRank = EducationLevel.fromString(female.getInterestEducation()).ordinal();
      String interestRelationshipGoal = female.getRelationshipGoal();

      for (int age = minAge; age <= maxAge; age++) {
        for (int eduRank = interestEducationRank; eduRank <= EducationLevel.values().length; eduRank++) {
          String key = age + "-" + interestEthnicity + "-" + eduRank + "-" + interestRelationshipGoal;
          if (malesGrouped.containsKey(key)) {
            List<Person> maleMatch = malesGrouped.get(key);
            fmatches.addAll(maleMatch);
          }
        }
      }
    }
  }
}
