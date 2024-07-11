import java.io.IOException;

import static API.RandomAPI.callAPI;

public class Main {
  public static void main(String[] args) {
    // Call API to instantiate testing data.
    try {
      callAPI();
    } catch (IOException | InterruptedException e) {
      throw new RuntimeException(e);
    }

    // Call Algorithm class/ method below
  }
}
