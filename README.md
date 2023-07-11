### Descrizione 'hash_map.c/.h'
In questo file ho voluto definire una struttura che contenesse tutti i dati che potevano servire per la gestione di una hash map, ho inotre definito le varie funzioni che operano su questa hash map.

### Descrizione 'buffer.c/.h'
In questi file ho definito una struttura che rappresenta un buffer generico, quindi con mutex e condvar utili per la gestione della muta eslusione durante l'accesso al buffer da più thread. Ho inotre definito le funzione per operare in modo thread-safe sul buffer.

- **buffer_create()**
Descrizione: Questa funzione crea un nuovo buffer e restituisce un puntatore ad esso.
Parametri:
Nessun parametro.
Ritorno: buffer_t*
Puntatore al buffer creato.
Note: La funzione alloca memoria per la struttura buffer_t e per il buffer stesso. Inizializza anche il mutex e le variabili di condizione utilizzate per la sincronizzazione. Se si verificano errori durante l'allocazione o l'inizializzazione, vengono eseguite le pulizie necessarie e viene restituito NULL per indicare un errore.

- **buffer_insert(buffer_t *buffer, char *str)**
Descrizione: Questa funzione inserisce un elemento nel buffer.
Parametri:
buffer (Tipo: buffer_t*): Puntatore al buffer in cui inserire l'elemento.
str (Tipo: char*): Stringa da inserire nel buffer.
Ritorno: Nessun valore di ritorno.
Note: La funzione acquisisce il lock del mutex del buffer utilizzando pthread_mutex_lock(). Successivamente, utilizza una condizione di attesa (pthread_cond_wait()) per attendere fino a quando il buffer non è pieno. Una volta che il buffer non è più pieno, viene aggiunto l'elemento alla posizione corrente del buffer e vengono aggiornati gli indici di controllo. Infine, viene inviato un segnale (pthread_cond_signal()) che il buffer non è più vuoto, e viene rilasciato il lock del mutex (pthread_mutex_unlock()).

- **buffer_remove(buffer_t *buffer)**
Descrizione: Questa funzione rimuove un elemento dal buffer e lo restituisce come risultato.
Parametri:
buffer (Tipo: buffer_t*): Puntatore al buffer da cui rimuovere l'elemento.
Ritorno: char*
Puntatore all'elemento rimosso dal buffer.
Note: La funzione acquisisce il lock del mutex del buffer utilizzando pthread_mutex_lock(). Successivamente, utilizza una condizione di attesa (pthread_cond_wait()) per attendere fino a quando il buffer non contiene elementi. Una volta che il buffer non è più vuoto, viene prelevato l'elemento dalla posizione corrente del buffer, vengono aggiornati gli indici di controllo e viene inviato un segnale (pthread_cond_signal()) che il buffer non è più pieno. Infine, viene rilasciato il lock del mutex (pthread_mutex_unlock()) e il valore prelevato viene restituito.

- **buffer_destroy(buffer_t *buffer)**
Descrizione: Questa funzione distrugge il buffer e libera la memoria associata.
Parametri:
buffer (Tipo: buffer_t*): Puntatore al buffer da distruggere.
Ritorno: Nessun valore di ritorno.
Note: La funzione libera la memoria per ogni elemento nel buffer utilizzando free(). Successivamente, libera la memoria del buffer stesso e distrugge il mutex e le variabili di condizione utilizzate per la sincronizzazione. Infine, libera la memoria associata alla struttura del buffer.


### Descrizione 'thread.c/.h'
In questo file ho definito una struttura che potesse rappresentare in modo generico un thread, ho inoltre definito delle strutture data per i thread lettori e scrittori (o decisono di usare una sola versione essendo che a parte per il puntatore al file ed il suo mutex queste due tipologie di thread hanno bisogno degli stessi dati, per il file ed il suo mutex semplicemente li mantendo a NULL, nel momento in cui non li uso) e per il gestore dei segnali

### Descrizione 'connection.c/.h'
In questo file ho definito una struttura che rappresenta una connessione con due funzioni, una per creare e avviare la connessione ed una per stoppare e deallocare la struttura.