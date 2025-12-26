import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.TreeSet;

/*
    Esercizio: 
    Scrivere un programma Java che legge un elenco di nomi di file sulla linea di comando e stampa sul terminale l'elenco di questi file 
    con il numero di linee di testo in essi contenute. 
    Si veda il programma Terna.java per vedere come leggere (e quindi contare) le linee di un file. 
    L'elenco deve essere ordinato per numero di linee decrescente e a parità di numero di linee lessicograficamente per nome del file. 
    Se qualcuno dei file passati sulla linea di comando non esiste deve essere semplicemente ignoratoe non riportato nell'elenco.

*/


public class ContaLinee implements Comparable<ContaLinee> {
    int lines;
    String nome;

    public ContaLinee(int l, String nome) {
        this.lines = l; // numero di lineee del file
        this.nome = nome; // nome del file
    }

    @Override
    public String toString() {
        return String.format("[%s - totale_linee: %d] ", nome, lines);
    }

    public int compareTo(ContaLinee ct) {
        if (lines < ct.lines) return 1;
        else if (lines > ct.lines) return -1;
        else return nome.compareTo(ct.nome);
    }

    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Uso: java ContaLinee file1 file2 ... filen");
            System.exit(1);
        }
        // viene creato un set per contenere oggetti di tipo ContaLinee        
        TreeSet<ContaLinee> dict = new TreeSet<ContaLinee>();

        for (int i = 0; i < args.length; i++) {
            // contatore per linee
            int contatoreLinee = 0;
            // controllo esistenza del file
            File f = new File(args[i]);
            if (!f.exists() || !f.isFile()) {
                continue;
            }

            try (BufferedReader br = new BufferedReader(new FileReader(args[i]))) {
                while (br.readLine() != null) {
                    contatoreLinee++; // incrementa il contatore per ogni riga letta
                } 
                ContaLinee ct = new ContaLinee(contatoreLinee, args[i]); // viene creato un oggetto ContaLinee e aggiunto al set
                dict.add(ct);
            } catch (IOException e) {
                System.err.println("Error: " + e.getMessage());
                // traccia dello stack per il debug
                e.printStackTrace();
            }
        }
        for (ContaLinee cl: dict) {
            System.out.println(cl);
        }
    }
}
