/*
    classe per memorizzare un cammino di costo minimo da un nodo sorgente.
    Per mantenere la rappresentazione compatta, memorizzo il nodo precedente e il costo del cammino.
*/


public class Cammino {
    Nodo precedente = null;
    double costo = Double.NaN;

    public Cammino(Nodo p, double c) {
        precedente = p;
        costo = c;
    }

    public String toString() {
        return String.format("%5->2f", costo, precedente.etichetta());
    }
}



