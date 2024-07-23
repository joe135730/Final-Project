package People;

public enum EducationLevel {
  HIGH_SCHOOL("High School", 1),
  BACHELORS("Balchelor's", 2),
  MASTERS("Master's", 3),
  DOCTORATE("Doctorate", 4);

  private final String name;
  private final int rank;

  EducationLevel(String name, int rank) {
    this.name = name;
    this.rank = rank;
  }

  public String getName() {
    return name;
  }

  public int getRank() {
    return rank;
  }

  public static EducationLevel fromString(String text) {
    for (EducationLevel level : EducationLevel.values()) {
      if (level.getName().equalsIgnoreCase(text)) {
        return level;
      }
    }
    throw new IllegalArgumentException("No constant with text " + text + " found");
  }
}
