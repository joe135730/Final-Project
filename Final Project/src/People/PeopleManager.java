package People;

import java.util.ArrayList;
import java.util.List;

import People.Person;

public class PeopleManager {
  private final List<Person> maleList;
  private final List<Person> femaleList;

  public PeopleManager() {
    maleList = new ArrayList<>();
    femaleList = new ArrayList<>();
  }

  public void addPerson(Person person) {
    if ("Male".equalsIgnoreCase(person.getSex())) {
      maleList.add(person);
    } else if ("Female".equalsIgnoreCase(person.getSex())) {
      femaleList.add(person);
    }
  }

  public List<Person> getMaleList() {
    return maleList;
  }

  public List<Person> getFemaleList() {
    return femaleList;
  }
}
