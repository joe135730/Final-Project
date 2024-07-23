import java.io.IOException;

import Algorithm.Filter;
import People.PeopleManager;
import People.Person;

import static API.RandomAPI.callAPI;

public class Main {
  public static void main(String[] args) {
    // Instantiate PeopleManager
    PeopleManager pm = new PeopleManager();

    // Call API to instantiate testing data.
    try {
      callAPI(pm);
    } catch (IOException | InterruptedException e) {
      throw new RuntimeException(e);
    }

    // Call Algorithm class/ method below
    // Instantiate Filter and apply filtering
    Filter filter = new Filter();
    filter.addPotentialCandidatesMale(pm.getMaleList(), pm.getFemaleList());
    filter.addPotentialCandidatesFemale(pm.getMaleList(), pm.getFemaleList());

    boolean isEmptyPotentialList = false; // track to see if person has empty potential list
    // Male Profiles
    // Comment/ Uncomment to check list output
    System.out.println("Male Profiles:");
    for (Person male : pm.getMaleList()) {
      System.out.println("ID: " + male.getId() + "\n" +
              "Name: " + male.getName() + "\n" +
              "Age: " + male.getAge() + "\n" +
              "InterestAge: Min: " + male.getInterestAge()[0] + ", Max: " + male.getInterestAge()[1] +"\n" +
              "Sex: " + male.getSex() + "\n" +
              "Interest: " + male.getInterest() + "\n" +
              "Ethnicity: " + male.getEthnicity() + "\n" +
              "Interest Ethnicity: " + male.getInterestEthnicity() + "\n" +
              "Religion: " + male.getReligion() + "\n" +
              "Education: " + male.getEducation() + "\n" +
              "Interest Education: " + male.getInterestEducation() + "\n" +
              "Relationship Goal: " + male.getRelationshipGoal() + "\n" +
              "Coordinates: x: " + male.getCoordinates()[0] +
              ", y: " + male.getCoordinates()[1] + "\n");

      // Check if potential candidates are not empty
      if (!male.getPotentialCandidates().isEmpty()) {
        System.out.println("Potential Matches:");
        for (Person match : male.getPotentialCandidates()) {
          System.out.println("\tID: " + match.getId() + " - Name: " + match.getName());
        }
      }
      else {
        System.out.println("Potential Matches: None");
        isEmptyPotentialList = true;
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
      if (!female.getPotentialCandidates().isEmpty()) {
        System.out.println("Potential Matches:");
        for (Person match : female.getPotentialCandidates()) {
          System.out.println("\tID: " + match.getId() + " - Name: " + match.getName());
        }
      }
      else {
        System.out.println("Potential Matches: None");
        isEmptyPotentialList = true;
      }
      System.out.println();
    }

    // Comment/ Uncomment to see if users has empty potential list
    if (isEmptyPotentialList) {
      System.out.println("There are users have empty potential list");
    }
    else {
      System.out.println("Every user has potential list");
    }
  }
}
