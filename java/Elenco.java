/*
 * Scrivere un programma Java Elenco.java contenente la classe Elenco che una volta eseguita:

    legge dalla linea di comando il nome di un file di testo e lo apre.
    legge tutte le linee del file di testo e le memorizza in un Set in modo da eliminare i duplicati.
    trasferisce le linee rimaste in un array e le ordina per lunghezza crescente, le linee della stessa lunghezza devono essere ordinate lessicograficamente.
    stampa le linee ordinate dell'array su stdout con la funzione System.out.println().
*/

import java.util.List;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

class Elenco {
    Set<String> lines;
    
    public Elenco(Set<String> l) {
        this.lines = l;
    }

    @Override
    public String toString() {
    return String.format("total lines: %d", lines.size());
    } 

    public Set<String> getLines() {
        return this.lines;
    }

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: <program_file> <file_name>");
            System.exit(1);
        }

        Set<String> lineSet = new HashSet<>();
    
        try (BufferedReader br = new BufferedReader(new FileReader(args[0]))) {
            String line;
            while ((line = br.readLine()) != null) {
                lineSet.add(line);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        // add lines to an array then sort it
        List<String> lines_array = new ArrayList<>();
        for (String s: lineSet) {
            lines_array.add(s);
        }
        
        // Sort the array and printing out its content on stdout
        Collections.sort(lines_array, (s1, s2) -> s1.length() - s2.length()); 
        for (String l : lines_array) {
            System.out.println(l);
        }
    }
}
