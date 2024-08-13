package People;

import java.util.ArrayList;
import java.util.List;

/**
 * The person class stores each person data.
 */
public class Person {
  private String id;
  private String name;
  private int age;
  private int[] interestAge; // Index 0: Min, Index 1: Max
  private String sex;
  private String interest;
  private String ethnicity;
  private String interestEthnicity;
  private String religion;
  private String education;
  private String interestEducation;
  private String relationshipGoal;
  private String coordinates;
  private List<Person> preferCandidates = new ArrayList<>();
  private List<Person> potentialCandidates = new ArrayList<>();

  /**
   * Gets id.
   *
   * @return the id
   */
// Getters
  public String getId() {
    return id;
  }

  /**
   * Gets name.
   *
   * @return the name
   */
  public String getName() {
    return name;
  }

  /**
   * Gets sex.
   *
   * @return the sex
   */
  public String getSex() {
    return sex;
  }

  /**
   * Gets age.
   *
   * @return the age
   */
  public int getAge() {
    return age;
  }

  public int[] getInterestAge() {
    return interestAge;
  }

  /**
   * Gets interest.
   *
   * @return the interest
   */
  public String getInterest() {
    return interest;
  }

  public String getEthnicity() {
    return ethnicity;
  }

  public String getInterestEthnicity() {
    return interestEthnicity;
  }

  public String getReligion() {
    return religion;
  }

  public String getEducation() {
    return education;
  }

  public String getInterestEducation() {
    return interestEducation;
  }

  public String getRelationshipGoal() {
    return relationshipGoal;
  }

  /**
   * Transfer String coordinates to double[] and return it.
   *
   * @return the double [ ]
   */
  public double[] getCoordinates() {
    String[] stringCoordinates = coordinates.split(",");
    // Parse String coordinates to double
    double[] coords = new double[2];
    // Parse the latitude and longitude values
    coords[0] = Double.parseDouble(stringCoordinates[0].trim()); // Trim whitespace and parse the latitude
    coords[1] = Double.parseDouble(stringCoordinates[1].trim()); // Trim whitespace and parse the longitude
    return coords;
  }

  public List<Person> getPreferCandidates() {
    return preferCandidates;
  }

  public List<Person> getPotentialCandidates() {
    return potentialCandidates;
  }

// Add setters if necessary for Gson to work properly
  /**
   * Sets id.
   *
   * @param id the id
   */
  public void setId(String id) {
    this.id = id;
  }

  /**
   * Sets name.
   *
   * @param name the name
   */
  public void setName(String name) {
    this.name = name;
  }

  /**
   * Sets age.
   *
   * @param age the age
   */
  public void setAge(int age) {
    this.age = age;
  }

  public void setInterestAge(int[] interestAge) {
    this.interestAge = interestAge;
  }

  /**
   * Sets sex.
   *
   * @param sex the sex
   */
  public void setSex(String sex) {
    this.sex = sex;
  }

  /**
   * Sets interest.
   *
   * @param interest the interest
   */
  public void setInterest(String interest) {
    this.interest = interest;
  }

  public void setEthnicity(String ethnicity) {
    this.ethnicity = ethnicity;
  }

  public void setInterestEthnicity(String interestEthnicity) {
    this.interestEthnicity = interestEthnicity;
  }

  public void setReligion(String religion) {
    this.religion = religion;
  }

  public void setEducation(String education) {
    this.education = education;
  }

  public void setInterestEducation(String interestEducation) {
    this.interestEducation = interestEducation;
  }

  public void setRelationshipGoal(String relationshipGoal) {
    this.relationshipGoal = relationshipGoal;
  }

  /**
   * Sets coordinates.
   *
   * @param coordinates the coordinates
   */
  public void setCoordinates(String coordinates) {
    this.coordinates = coordinates;
  }

  public void setPreferCandidates(List<Person> preferCandidates) {
    this.preferCandidates = preferCandidates;
  }

  public void addPotentialCandidate(Person candidate) {
    if (!potentialCandidates.contains(candidate)) {
      potentialCandidates.add(candidate);
    }
  }

  public void addPreferCandidate(Person person){
    if(preferCandidates == null) preferCandidates = new ArrayList<>();
    if(!preferCandidates.contains(person)){
      preferCandidates.add(person);
    }
  }
}
