package API;

import java.util.ArrayList;
import java.util.List;

public class PeopleManager {
  private static List<Person> maleList = new ArrayList<>();
  private static List<Person> femaleList = new ArrayList<>();

  public static void addPerson(Person person) {
    if ("Male".equalsIgnoreCase(person.getSex())) {
      maleList.add(person);
    } else if ("Female".equalsIgnoreCase(person.getSex())) {
      femaleList.add(person);
    }
  }

  public static List<Person> getMaleList() {
    return maleList;
  }

  public static List<Person> getFemaleList() {
    return femaleList;
  }
}
