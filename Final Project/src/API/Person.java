package API;

/**
 * The person class stores each person data.
 */
public class Person {
  private String id;
  private String name;
  private int age;
  private int interestAge;
  private String sex;
  private String interest;
  private String ethnicity;
  private String interestEthnicity;
  private String religion;
  private String education;
  private String relationshipGoal;
  private String coordinates;
  private int rating;

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

  /**
   * Gets interest.
   *
   * @return the interest
   */
  public String getInterest() {
    return interest;
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

  public int getRating() {
    return rating;
  }

  /**
   * Sets id.
   *
   * @param id the id
   */
// Add setters if necessary for Gson to work properly
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

  /**
   * Sets coordinates.
   *
   * @param coordinates the coordinates
   */
  public void setCoordinates(String coordinates) {
    this.coordinates = coordinates;
  }

  // Add abcdefg
}
