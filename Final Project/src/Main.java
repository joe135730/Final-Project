import java.io.IOException;

import Algorithm.Algorithm;
import People.PeopleManager;

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
    Algorithm.executeAlgorithm(pm);
  }
}
