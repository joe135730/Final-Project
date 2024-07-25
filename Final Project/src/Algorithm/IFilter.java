package Algorithm;

import java.util.List;

import People.Person;

public interface IFilter {
   void addPreferCandidatesMale(List<Person> males, List<Person> females);
   void addPreferCandidatesFemale(List<Person> males, List<Person> females);
}
