package API;

import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.google.gson.reflect.TypeToken;
import java.io.IOException;
import java.lang.reflect.Type;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.util.List;

import People.PeopleManager;
import People.Person;

/**
 * The Random api class call api from RandomAPI and transfer Json data into Java ArrayList
 * URL: https://randomapi.com/
 * Cheyi Wu's key:
 * api key: SPP1-8JT9-WDHY-RKW9
 * ref key: atopjxhd
 */
public class RandomAPI {
  /**
   * Call api.
   *
   * @throws IOException          the io exception
   * @throws InterruptedException the interrupted exception
   */
  public static void callAPI(PeopleManager pm) throws IOException, InterruptedException {
    // API String (Change this by creating an account, else the usage will be used up quickly.)
    String API = "https://randomapi.com/api/?key=SPP1-8JT9-WDHY-RKW9&ref=atopjxhd&results=1&fmt=pretty&noinfo";
    // Create a HttpClient
    HttpClient client = HttpClient.newHttpClient();
    // Prepare the request
    HttpRequest request = HttpRequest.newBuilder()
            .uri(URI.create(API)) // Replace with personal API URL and parameters
            .GET() // GET request
            .build();

    // Send the request and receive the response
    HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
    String responseBody = response.body();

    // Comment/ uncomment to check original json response
    // System.out.println("JSON Response: " + responseBody);

    // Use Gson to parse the JSON response
    Gson gson = new Gson();
    JsonObject jsonResponse = JsonParser.parseString(responseBody).getAsJsonObject();
    JsonArray jsonProfiles = jsonResponse.getAsJsonArray("results").get(0).getAsJsonObject().getAsJsonArray("profiles");

    // Type token
    Type listType = new TypeToken<List<Person>>() {}.getType();
    List<Person> profiles = gson.fromJson(jsonProfiles, listType);

    // Categorize profiles into male and female lists
    for (Person profile : profiles) {
      pm.addPerson(profile);
    }
  }
}
