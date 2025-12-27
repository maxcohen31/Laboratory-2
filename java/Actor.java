import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Set;
import java.util.HashSet;
import java.util.Map;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;


public class Actor implements Comparable<Actor> {
    String name;
    Set<String> professions;

    public Actor(String n, Set<String> prof) {
        this.name = n;
        this.professions = prof;
    }

    public Actor(String line) {
        String[] fields = line.split("\t"); // split on columns
        if (fields.length != 6) {
            throw new IllegalArgumentException("Line has less than 6 fields: " + line);
        }
        name = fields[1].trim(); // get name
        professions = new HashSet<String>(Arrays.asList(fields[4].split(","))); // get professions
    }

    @Override
    public String toString() {
        return String.format("Name: %s - Profession: %s", name, professions);
    }
    
    @Override
    public int compareTo(Actor ac) {
        return name.compareTo(ac.name);
    }

    public String getName() {
        return name;
    }

    public Set<String> getProfessions() {
        return professions;
    }

    /**
     * Prints out the association <name, list of professions>
     *
     * @param m map <name, list of professions>
    */
    public static void printMap(Map<String, Set<String>> m) {
        for (Map.Entry<String, Set<String>> entry: m.entrySet()) {
            System.out.println(entry.getKey() + ": " + entry.getValue());
        }
    }

    /**
     * Writes to a file "actresses.txt" all the people who have 'actress' in their professions.
     *
     * @param m Map with association <person name, set of professions>
    */
    public static void writeActresses(Map<String, Set<String>> m) {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter("actresses.txt"))) { 
            for (Map.Entry<String, Set<String>> entry: m.entrySet()) {
                    Set<String> prof = entry.getValue();
                    if (prof.contains("actress")) {
                        writer.write(entry.getKey());
                        writer.newLine();
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    /**
     * Sorts the contents of the given file alphabetically and writes them to "sorted_actresses.txt".
     *
     * @param file File object pointing to the input file
    */
    public static void sortFile(File file) {
        List<String> names = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = br.readLine()) != null) {
                names.add(line);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        Collections.sort(names);

        try (BufferedWriter writer = new BufferedWriter(new FileWriter("sorted_actresses.txt"))) {
            for (String s: names) {
                writer.write(s);
                writer.newLine();
            }
        } catch (IOException e) {
            e.printStackTrace();
        } 
    }

    /**
     * Writes one file per profession, containing the names of all people with that profession.
     * Each file is named "<profession>.txt".
     *
     * @param m Map with association <profession, set of person names>
    */
    public static void writeProfession(Map<String, Set<String>> m) {
        for (Map.Entry<String, Set<String>> entry: m.entrySet()) {
            String p = entry.getKey();
            Set<String> person = entry.getValue();

            try (BufferedWriter writer = new BufferedWriter(new FileWriter(p + ".txt"))) {
                List<String> sorted = new ArrayList<>(person);   
                Collections.sort(sorted);
                for (String s: sorted) {
                    writer.write(s);
                    writer.newLine();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("### Error: missing file ###");
            System.exit(1);
        }

        Map<String, Set<String>> ruoli = new HashMap<>(); // <person, profesions>
        Map<String, Set<String>> personProfession = new HashMap<>(); // <profession, persons>
        try (BufferedReader br = new BufferedReader(new FileReader(args[0]))) {
            br.readLine(); // skip the first line
            String l;
            while ((l= br.readLine()) != null) {
               Actor ac = new Actor(l); 
               ruoli.put(ac.getName(), ac.getProfessions());
               // populate the personProfesion map
               for (String prof: ac.getProfessions()) {
                   personProfession.computeIfAbsent(prof, k -> new HashSet<>()).add(ac.getName()); // create a set if not existing
               }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } 
    }
}
