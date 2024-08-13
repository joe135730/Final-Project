package Algorithm;

import java.util.ArrayList;
import java.util.List;

import People.PeopleManager;
import People.Person;

public class Algorithm {
  // Algorithm starts here
  public static void executeAlgorithm(PeopleManager pm) {
    // Instantiate Filter and preferCandidates
    Filter filter = new Filter();
    filter.addPreferCandidatesMale(pm.getMaleList(), pm.getFemaleList());
    filter.addPreferCandidatesFemale(pm.getMaleList(), pm.getFemaleList());

    // Instantiate Graph for graph algorithm by adding male and female list in one.
    Graph g = new Graph(pm);

    boolean isEmptyPreferList = false; // track to see if person has empty potential list
    // Male Profiles
    // Comment/ Uncomment to check list output
    System.out.println("Male Profiles:");
    for (Person male : pm.getMaleList()) {
      System.out.println("ID: " + male.getId() + "\n" +
              "Name: " + male.getName() + "\n" +
              "Age: " + male.getAge() + "\n" +
              "Sex: " + male.getSex() + "\n" +
              "Interest: " + male.getInterest() + "\n" +
              "Ethnicity: " + male.getEthnicity() + "\n" +
              "Religion: " + male.getReligion() + "\n" +
              "Education: " + male.getEducation() + "\n" +
              "InterestAge: Min: " + male.getInterestAge()[0] + ", Max: " + male.getInterestAge()[1] +"\n" +
              "Interest Ethnicity: " + male.getInterestEthnicity() + "\n" +
              "Interest Education: " + male.getInterestEducation() + "\n" +
              "Relationship Goal: " + male.getRelationshipGoal() + "\n" +
              "Coordinates: x: " + male.getCoordinates()[0] +
              ", y: " + male.getCoordinates()[1] + "\n");

      // Check if potential candidates are not empty
      if (!male.getPreferCandidates().isEmpty()) {
        System.out.println("Potential Matches:");
        for (Person match : male.getPreferCandidates()) {
          System.out.println("\tID: " + match.getId() + " - Name: " + match.getName());
        }
      }
      else {
        System.out.println("Potential Matches: None");
        isEmptyPreferList = true;
      }
      System.out.println();
    }

    // Female Profiles
    // Comment/ Uncomment to check list output
    System.out.println("Female Profiles:");
    for (Person female : pm.getFemaleList()) {
      System.out.println(
              "ID: " + female.getId() + "\n" +
                      "Name: " + female.getName() + "\n" +
                      "Age: " + female.getAge() + "\n" +
                      "InterestAge: Min:" + female.getInterestAge()[0] + ", Max: " + female.getInterestAge()[1] + "\n" +
                      "Sex: " + female.getSex() + "\n" +
                      "Interest: " + female.getInterest() + "\n" +
                      "Ethnicity: " + female.getEthnicity() + "\n" +
                      "Interest Ethnicity: " + female.getInterestEthnicity() + "\n" +
                      "Religion: " + female.getReligion() + "\n" +
                      "Education: " + female.getEducation() + "\n" +
                      "Interest Education: " + female.getInterestEducation() + "\n" +
                      "Relationship Goal: " + female.getRelationshipGoal() + "\n" +
                      "Coordinates: x: " + female.getCoordinates()[0] +
                      ", y: " + female.getCoordinates()[1]);

      // Check if potential candidates are not empty
      if (!female.getPreferCandidates().isEmpty()) {
        System.out.println("Prefer Matches:");
        for (Person match : female.getPreferCandidates()) {
          System.out.println("\tID: " + match.getId() + " - Name: " + match.getName());
        }
      }
      else {
        System.out.println("Prefer Matches: None");
        isEmptyPreferList = true;
      }
      System.out.println();
    }

    // Comment/ Uncomment to see if users has empty potential list
    if (isEmptyPreferList) {
      System.out.println("There are users have empty potential list");
    }
    else {
      System.out.println("Every user has potential list");
    }
  }
}
