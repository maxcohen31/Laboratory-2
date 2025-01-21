import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
      Classe contentente il metodo main per leggere un file di città
      e costruire un grafo in cui i nodi sono città e gli archi
      sono coppie di città con distanza inferiore a un certo limite
      
      Dopo la costruzione del grafo, calcola i cammini minimi
      dalla prima città del file a tutte le altre città
 */

public class GrafoCitta {
    /**
      Metodo main per leggere un file di città e costruire un grafo
      legge dalla linea di comando il nome di un file e un limite di distanza in chilometri
    */

    public static void main(String[] args) {
        if (args.length != 2) {
            System.out.println("Uso: java GrafoCitta nomefile");
            System.exit(1);
        }      

    // crea lista città
    List<Citta> elenco = new ArrayList<Citta>();
    try {
        BufferedReader br = new BufferedReader(new FileReader(args[0]));
        String linea;

        while ((linea = br.readLine()) != null) {
            // salta linee vuote o commenti
            if (linea.length() == 0 || linea.charAt(0) == '#') {
                continue;
            }
            Citta c = new Citta(linea);
            if (elenco.contains(c)) {
                System.out.println("Città duplicata: " + c);
            }
            else {
                elenco.add(c);
            }
            br.close();
        }
    }
    catch (Exception e) {
        System.err.println("Errore: " + e);
        e.printStackTrace();
        System.exit(2);
    }

    // inserisce le citta nel grafo come nodi
    LogGrafo g = new LogGrafo();
    for (Citta c: elenco) {
        g.aggiungi_nodo(c);
    }
    // inserisce gli archi nel grafo
    // considero tutte le coppie di città distinte
    // e inserico un arco se la distanza è minore del limite
    double limite = Double.parseDouble(args[1]);
    for (int i=0; i<elenco.size(); i++) {
      Citta c1 = elenco.get(i);
      for(int j=i+1; j<elenco.size(); j++) {
        Citta c2 = elenco.get(j);
        double dist = distanza(c1,c2);
        if(dist <= limite)
          g.aggiungi_arco(new Arco(c1,c2,dist));
      }
    }
    // calcolo i cammini minimi dalla prima città dell'elenco (Roma)
    Citta sorgente = elenco.get(0); 
    Map<Nodo,Cammino> m = g.dijkstra(sorgente);
    for(Nodo n : m.keySet()) {
      System.out.println(n.etichetta() + " " + m.get(n));
    }
    System.out.println("Ci sono " + m.size() + " città raggiungibili da " + sorgente.etichetta()  ); 
  }
        

    /** 
   * Calcola la distanza fra due città
   * 
   * @param c1 prima città
   * @param c2 seconda città
   * @return distanza in chilometri
   *
    Per il calcolo viene utilizzata la formula di Haversine
    https://en.wikipedia.org/wiki/Haversine_formula  */
    public static double distanza(Citta c1, Citta c2) {
        double lat1 = Math.toRadians(c1.lat);
        double lat2 = Math.toRadians(c2.lat);
        double lon1 = Math.toRadians(c1.lon);
        double lon2 = Math.toRadians(c2.lon);
        double dlat = lat2 - lat1;
        double dlon = lon2 - lon1;
        double a = Math.pow(Math.sin(dlat/2),2) + Math.cos(lat1)*Math.cos(lat2)*Math.pow(Math.sin(dlon/2),2);
        double c = 2*Math.atan2(Math.sqrt(a),Math.sqrt(1-a));
        return 6371*c;
    }
}


