package People;

import java.util.ArrayList;
import java.util.List;

import People.Person;

/**
 * The PeopleManager class is responsible for managing the collection of Person objects,
 * segregating Person objects into separate lists based on their sex.
 */
public class PeopleManager {
  // List for holding male persons
  private final List<Person> maleList;
  // List for holding female persons
  private final List<Person> femaleList;

  /**
   * PeopleManager constructor that initializes the lists for male and female Persons.
   */
  public PeopleManager() {
    // Initialize maleList ArrayList for storing objects identified as male
    maleList = new ArrayList<>();
    // Initialize femaleList ArrayList for storing objects identified as female
    femaleList = new ArrayList<>();
  }

  /**
   * Adds a Person object to the list based on the person's sex.
   *
   * @param person The Person object to be added
   */
  public void addPerson(Person person) {
    if ("Male".equalsIgnoreCase(person.getSex())) {
      maleList.add(person);
    } else if ("Female".equalsIgnoreCase(person.getSex())) {
      femaleList.add(person);
    }
  }

  /**
   * Returns the list of all male Persons.
   *
   * @return A List of Person objects who are identified as male
   */
  public List<Person> getMaleList() {
    return maleList;
  }

  /**
   * Returns the list of all female Persons.
   *
   * @return A List of Person objects who are identified as female
   */
  public List<Person> getFemaleList() {
    return femaleList;
  }
}
