import java.io.BufferedReader;
import java.io.FileReader;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.Set;
import java.util.TreeSet;
import java.util.Comparator;
import java.util.HashMap;

// eccezione custom 
class IncompleteRow extends RuntimeException {
    IncompleteRow(String e) {
        super(e);
    }
}

// comparatore - confronta le nazioni 
// se la nazione è la stessa allora confronta le città
class NationComparator implements Comparator<CittaSudAmerica> {
    @Override
    public int compare(CittaSudAmerica cs1, CittaSudAmerica cs2) {
        if (cs1.nazione.compareTo(cs2.nazione) == 0) {
            return cs1.nome.compareTo(cs2.nome);
        }
        return cs1.nazione.compareTo(cs2.nazione);
    }
} 

public class CittaSudAmerica implements Nodo {
    String nome;
    int popolazione;
    String nazione;

    // Costruttore 
    public CittaSudAmerica(String nome, int popolazione, String nazione)
    {
        this.nome = nome;
        this.popolazione = popolazione;
        this.nazione = nazione;
    }

    // converte una stringa in una città se è possibile
    public CittaSudAmerica(String s)
    {
        String[] campi = s.split(";");
        if (campi.length != 3) {
            throw new IncompleteRow("Riga con campi diversi da 3");
        }
        nome = campi[0].trim();
        popolazione = Integer.parseInt(campi[1].trim());
        nazione = campi[2].trim();
    }

    @Override
    public boolean equals(Object obj) {
// verifica se obj == null oppure se obj non è una città
        if (obj == null || obj.getClass() != getClass()) {
            return false;
        }
        CittaSudAmerica cs = (CittaSudAmerica) obj;
        return nome.equals(cs.nome) && popolazione == cs.popolazione && nazione.equals(cs.nazione); 
    }

    @Override
    public String toString() {
        return String.format("%s, popolazione: %d, stato: %s", nome, popolazione, nazione);
    }
    
    // override del metodo hashCode per garantire coerenza con equals
    // se due oggetti sono uguali, devono avere lo stesso hashcode
    // necessario altrimenti HashSet non funziona correttamente
    // notare che la qualità del codice hash non è importante
    // per la correttezza del programma, ma influisce sulle prestazioni
    // ce ne accorgiamo quando abbiamo molti oggetti perché
    // un buon hashcode garantisce operazioni in tempo costante
    // uno cattivo può portare a operazioni in tempo lineare
    @Override
    public int hashCode() {
        int hash = 7;
        hash = 31 + hash + nome.hashCode();
        hash = 31 + hash + Integer.hashCode(popolazione);
        hash = 31 + hash + nazione.hashCode();
        return hash;
    }

    public int compareTo(CittaSudAmerica cs) {
        return nome.compareTo(cs.nome);
    }

    public String etichetta() {
        return nome;
    }
    
    
    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Uso: java CittaSudAmerica nomefile");
            System.exit(1);
        }

        // crea un insieme di CittaSudAmerica
        Set<CittaSudAmerica> setCitta = new HashSet<CittaSudAmerica>();
        try {
            BufferedReader br = new BufferedReader(new FileReader(args[0]));
            String rigaLetta;

            while ((rigaLetta = br.readLine()) != null) {
                // salta linea vuote o commenti
                if (rigaLetta.length() == 0 || rigaLetta.charAt(0) == '#') {
                    continue;
                }
                // creo una nuova CittaSudAmerica 
                CittaSudAmerica c = new CittaSudAmerica(rigaLetta);
                if (!(setCitta.add(c))) {
                    System.out.println("Città già inserita in precedeza: " + c);
                }
            }
            br.close();
        }
        catch (IncompleteRow e) {
            System.out.println(e + "interrompo la lettura");
            
        }
        catch (Exception e) {
            System.out.println("Error: " + e);
            e.printStackTrace();
        }

        // ordinamento di setCitta basato sul nome della nazione
        // se la nazione di due città è la medesima si ordina in base alla città
        TreeSet<CittaSudAmerica> csts = new TreeSet<>(new NationComparator());
        csts.addAll(setCitta);
        // stampa elenco città ordinate per nazione
        System.out.println("#### Start ####");
        for (CittaSudAmerica cs : csts) {
            System.out.println(cs);
        }
        System.out.println("#### End ####");

        // conta la popolazione presente in ogni nazione
        HashMap<String, Integer> popMap = new LinkedHashMap<String, Integer>(); 
        for (CittaSudAmerica cs: csts) {
            String nation = cs.nazione;
            popMap.put(nation, popMap.getOrDefault(nation, 0) + cs.popolazione);
        }
        // stampo la coppia nazione - popolazione
        System.out.println("+----- Popolazione per nazione -----+");
        popMap.forEach((nat, value) -> System.out.println(nat + ": " + value)); 
    } 
}




