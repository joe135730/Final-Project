package Algorithm;

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
    BipartiteMatching bm = new BipartiteMatching(pm);
    bm.bipartiteMatching();

    int noPreferCount = 2000; // track to see if person has empty prefer list
    int noPotentialCount = 2000; // track to see if person has empty potential list

    boolean isEmptyPreferList = false; // track to see if person has empty potential list
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

      // Check if Prefer candidates are not empty
      if (!male.getPreferCandidates().isEmpty()) {
        System.out.println("Prefer Matches:");
        for (Person match : male.getPreferCandidates()) {
          int weights = bm.calculateWeight(male, match);
          System.out.println("\tID: " + match.getId() + " - Name: " + match.getName() + ", Weights: " + weights);
        }
      }
      else {
        System.out.println("Prefer Matches: None");
        noPreferCount--;
      }
      System.out.println();

      // Check if Potential candidates are not empty
      if (!male.getPotentialCandidates().isEmpty()) {
        System.out.println("Potential Matches:");
        for (Person match : male.getPotentialCandidates()) {
          int weights = bm.calculateWeight(male, match);
          System.out.println("\tID: " + match.getId() + " - Name: " + match.getName() + ", Weights: " + weights);
        }
      }
      else {
        System.out.println("Potential Matches: None");
        noPotentialCount--;
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

      // Check if Prefer candidates are not empty
      if (!female.getPreferCandidates().isEmpty()) {
        System.out.println("Prefer Matches:");
        for (Person match : female.getPreferCandidates()) {
          int weights = bm.calculateWeight(female, match);
          System.out.println("\tID: " + match.getId() + " - Name: " + match.getName() + ", Weights: " + weights);
        }
      }
      else {
        System.out.println("Prefer Matches: None");
        noPreferCount--;
      }
      System.out.println();

      // Check if Potential candidates are not empty
      if (!female.getPotentialCandidates().isEmpty()) {
        System.out.println("Potential Matches:");
        for (Person match : female.getPotentialCandidates()) {
          int weights = bm.calculateWeight(female, match);
          System.out.println("\tID: " + match.getId() + " - Name: " + match.getName() + ", Weights: " + weights);
        }
      }
      else {
        System.out.println("Potential Matches: None");
        noPotentialCount--;
      }
      System.out.println();
    }

    // Comment/ Uncomment to see if users has empty potential list
    System.out.println("Prefer Candidates Match Ratio: " + noPreferCount + "/" + 2000);
    System.out.println("Potential Candidates Match Ratio: " + noPotentialCount + "/" + 2000);
  }
}
