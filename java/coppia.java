import java.util.ArrayList;


class Coppia {
    int x;
    int y;

    /* costruttore Coppia */
    Coppia(int x, int y) {
        this.x = x;
        this.y = y;
    }

    /* costruttore vuoto */
    Coppia() {
        x = 0;
        y = 0;
    }

    public String toString() {
        return String.format("(%d, %d)", x, y);
    }

    public boolean equals(Coppia c) {
        return (x == c.x) && (y == c.y); /* non è il miglior modo per il confronto */
                                
    }

    public static void main(String[] args) {
        if (args.length < 2 || args.length % 2 == 1) {
            System.out.println("Uso: java Coppia x1 y1 x2 y2 ... " );
            System.exit(1);
        }

        /* crea una lista di coppie di interi passati sulla linea di comando */
        ArrayList<Coppia> lista = new ArrayList<Coppia>();
        for (int i = 0; i < args.length; i += 2) {
            int x = Integer.parseInt(args[i]);
            int y = Integer.parseInt(args[i+1]);
            lista.add(new Coppia(x, y));
        }
        lista.add(new Coppia()); /* aggiungo coppia (0, 0) */

        /* ordino lista */
        lista.sort((c1, c2) -> Math.abs(c1.x - c2.x) - Math.abs(c1.y - c2.y));


        /* ordina per differenza crescente e a parità di differenza, ordina per x crescente */
        lista.sort((c1, c2) -> 
                {
                    int diff1 = Math.abs(c1.x - c2.x);
                    int diff2 = Math.abs(c1.y - c2.y);
                    if (diff1 != diff2) {
                        return diff1 - diff2;
                    } else {
                        return c2.x - c1.x;
                    }
                });

        /* stampa coppie */
        for (Coppia c: lista) {
            System.out.println(c);
        }

        Coppia c = new Coppia(2, 3); 
        System.out.println("c = " + c + "è in lista?" + lista.contains(c)); /* test del metodo contains */
    }
}
