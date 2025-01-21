/*
  Interfaccia per rappresentare un nodo di un grafo
  l'unica richiesta per il tipo nodo (attualmente) è che ogni nodo abbia un'etichetta che viene usata per identificarlo in modo univoco.
  Qualsiasi classe che ha il metodo etichetta() può essere in nodo di un grafo semplicemente implementado questa interfaccia.
*/


public interface Nodo {
    // nodo diversi devono avere etichetta diversa
    public String etichetta();
}
