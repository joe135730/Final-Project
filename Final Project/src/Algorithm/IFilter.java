package Algorithm;

import java.util.List;

import People.Person;

public interface IFilter {
   void addPotentialCandidatesMale(List<Person> males, List<Person> females);
   void addPotentialCandidatesFemale(List<Person> males, List<Person> females);
}
