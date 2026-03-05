// Compilazione:
//   gcc aslr.c -o aslr
//
// Scopo dell'esempio:
//   Mostrare l'effetto di ASLR (Address Space Layout Randomization), una misura di sicurezza del SO
//   che randomizza (sposta in modo pseudo-casuale) le aree di memoria di un processo a ogni esecuzione.
//   In pratica, a ogni run, indirizzi come stack, heap e librerie condivise possono cambiare.
//
// Come verificare lo stato di ASLR (Linux):
//   cat /proc/sys/kernel/randomize_va_space
//
// Valori tipici:
//   2 = randomizzazione completa (default su molte distro)
//   1 = randomizzazione conservativa (parziale)
//   0 = ASLR disabilitato
//
// Esperimento 1, ASLR attivo (valore 2 o 1):
//   Esegui più volte:
//     ./aslr
//   e osserva che gli indirizzi stampati cambiano tra una run e la successiva.
//   Nota: a seconda dell'architettura e della configurazione, alcune aree possono variare meno di altre.
//
// Esperimento 2, ASLR disabilitato (serve privilegio di amministratore):
//   Opzione A:
//     sudo bash -c "echo 0 > /proc/sys/kernel/randomize_va_space"
//   Opzione B:
//     sudo sysctl -w kernel.randomize_va_space=0
//
//   Ora avvia due istanze “quasi contemporanee”:
//     ./aslr & ./aslr
//   Se ASLR è davvero disabilitato, gli indirizzi stampati dai due processi risultano uguali
//   
//
// Importante:
//   Riabilita ASLR al termine (consigliato):
//     sudo sysctl -w kernel.randomize_va_space=2
//   Disabilitare ASLR riduce la sicurezza del sistema.
//

#include <unistd.h>
#include <stdio.h>

// Variabile globale (storage statico):
// - esiste per tutta la durata del programma
// - risiede nella sezione "data" del processo (o BSS se non inizializzata)
// - il suo indirizzo è tipicamente legato al layout del segmento dati del processo
int staticVar = 1;

int main() {
    // Variabile locale (stacked):
    //  con ASLR attivo, la posizione dello stack tende a cambiare tra una run e l'altra
    int localVar = 1;

    staticVar += 1;
    localVar += 1;

    // Utile per confrontare gli output di due processi contemporaneamente
    // sleep(10);

    printf("static address: %p, value: %d\n", (void*)&staticVar, staticVar);
    printf("local  address: %p, value: %d\n", (void*)&localVar,  localVar);

    return 0;
}

