package Algorithm;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import People.EducationLevel;
import People.Person;

/**
 * The class implements IFilter interface providing methods to add preferred candidates
 * for male/ female lists based on specified preferences and criteria.
 */
public class Filter implements IFilter {

  /**
   * Filters and adds preferred female candidates to each male's preferred list based on the
   * male's interests in age, ethnicity, education level, and relationship goal.
   *
   * @param males   List of male Person objects
   * @param females List of female Person objects
   */
  @Override
  public void addPreferCandidatesMale(List<Person> males, List<Person> females) {
    // Group all female candidate by age, ethnicity, education, and relationship goals
    Map<String, List<Person>> femalesGrouped = females.stream().collect(
            Collectors.groupingBy(female -> female.getAge() + "-" +
                    female.getEthnicity() + "-" +
                    EducationLevel.fromString(female.getEducation()).ordinal() + "-" +
                    female.getRelationshipGoal())
    );

    // Iterate over each male to find matching female candidates based on his preferences
    for (Person male : males) {
      List<Person> matches = new ArrayList<>();

      // Get age range from male's interest
      int minAge = male.getInterestAge()[0];
      int maxAge = male.getInterestAge()[1];
      // Get other interests including ethnicity, education rank and relationship goal
      String interestEthnicity = male.getInterestEthnicity();
      int interestEducationRank = EducationLevel.fromString(male.getInterestEducation()).ordinal();
      String interestRelationshipGoal = male.getRelationshipGoal();

      // Iterate through the age range to find matches
      for (int age = minAge; age <= maxAge; age++) {
        String key = age + "-" + interestEthnicity + "-" + interestEducationRank + "-" + interestRelationshipGoal;
        // Check if there are females matching the criteria
        if (femalesGrouped.containsKey(key)) {
          List<Person> femaleMatch = femalesGrouped.get(key);
          matches.addAll(femaleMatch);
        }
      }
      // Sets the matched candidates for the male
      male.setPreferCandidates(matches);

    }
  }

  /**
   * Filters and adds preferred male candidates to each female's preferred list based on the
   * female's interests in age, ethnicity, education level, and relationship goal.
   *
   * @param males   List of male Person objects
   * @param females List of female Person objects
   */
  public void addPreferCandidatesFemale(List<Person> males, List<Person> females) {
    // Group all male by age, ethnicity, education, and relationship goal
    Map<String, List<Person>> malesGrouped = males.stream().collect(
            Collectors.groupingBy(male -> male.getAge() + "-" +
                    male.getEthnicity() + "-" +
                    EducationLevel.fromString(male.getEducation()).ordinal() + "-" +
                    male.getRelationshipGoal())
    );

    // Iterate over each female to find matching male candidates based on her preferences
    for (Person female : females) {
      List<Person> fmatches = new ArrayList<>();

      // Get age range from female's interest
      int minAge = female.getInterestAge()[0];
      int maxAge = female.getInterestAge()[1];
      // Get other interests including ethnicity, education rank and relationship goal
      String interestEthnicity = female.getInterestEthnicity();
      int interestEducationRank = EducationLevel.fromString(female.getInterestEducation()).ordinal();
      String interestRelationshipGoal = female.getRelationshipGoal();

      // Iterate through the age and education range to find matches
      for (int age = minAge; age <= maxAge; age++) {
        for (int eduRank = interestEducationRank; eduRank <= EducationLevel.values().length; eduRank++) {
          String key = age + "-" + interestEthnicity + "-" + eduRank + "-" + interestRelationshipGoal;
          // Check if there are males matching the criteria
          if (malesGrouped.containsKey(key)) {
            List<Person> maleMatch = malesGrouped.get(key);
            fmatches.addAll(maleMatch);
          }
        }
      }
    }
  }
}
