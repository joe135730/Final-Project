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

    // Male Profiles
    // Comment/ Uncomment to check list output
//    System.out.println("Male Profiles:");
//    for (Person male : pm.getMaleList()) {
//      System.out.println("ID: " + male.getId() + "\n" +
//              "Name: " + male.getName() + "\n" +
//              "Age: " + male.getAge() + "\n" +
//              "InterestAge: Min: " + male.getInterestAge()[0] + ", Max: " + male.getInterestAge()[1] +"\n" +
//              "Sex: " + male.getSex() + "\n" +
//              "Interest: " + male.getInterest() + "\n" +
//              "Ethnicity: " + male.getEthnicity() + "\n" +
//              "Interest Ethnicity: " + male.getInterestEthnicity() + "\n" +
//              "Religion: " + male.getReligion() + "\n" +
//              "Education: " + male.getEducation() + "\n" +
//              "Interest Education: " + male.getInterestEducation() + "\n" +
//              "Relationship Goal: " + male.getRelationshipGoal() + "\n" +
//              "Coordinates: x: " + male.getCoordinates()[0] +
//              ", y: " + male.getCoordinates()[1]
//      );
//      System.out.println();
//    }

    // Female Profiles
    // Comment/ Uncomment to check list output
//    System.out.println("Female Profiles:");
//    for (Person female : pm.getFemaleList()) {
//      System.out.println(
//              "ID: " + female.getId() + "\n" +
//                      "Name: " + female.getName() + "\n" +
//                      "Age: " + female.getAge() + "\n" +
//                      "InterestAge: Min:" + female.getInterestAge()[0] + ", Max: " + female.getInterestAge()[1] + "\n" +
//                      "Sex: " + female.getSex() + "\n" +
//                      "Interest: " + female.getInterest() + "\n" +
//                      "Ethnicity: " + female.getEthnicity() + "\n" +
//                      "Interest Ethnicity: " + female.getInterestEthnicity() + "\n" +
//                      "Religion: " + female.getReligion() + "\n" +
//                      "Education: " + female.getEducation() + "\n" +
//                      "Interest Education: " + female.getInterestEducation() + "\n" +
//                      "Relationship Goal: " + female.getRelationshipGoal() + "\n" +
//                      "Coordinates: x: " + female.getCoordinates()[0] +
//                      ", y: " + female.getCoordinates()[1]
//      );
//      System.out.println();
//    }

  }
}
