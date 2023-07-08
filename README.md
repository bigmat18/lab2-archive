### Descrizione 'archivio.c'
In questo file ho impelementato le varie funzioni utilizzati dai thread che era richiesto di creare. Con anche il
sistema di gestione dei signals.

### Descrizione 'hash_map.c/.h'
In questo file ho voluto definire una struttura che contenesse tutti i dati che potevano servire per la gestione di una hash map, ho inotre definito le varie funzioni che operano su questa hash map.

### Descrizione 'buffer.c/.h'
In questi file ho definito una struttura che rappresenta un buffer generico, quindi con mutex e condvar utili per la gestione della muta eslusione durante l'accesso al buffer da più thread. Ho inotre definito le funzione per operare in modo thread-safe sul buffer.

### Descrizione 'thread.c/.h'
In questo file ho definito una struttura che potesse rappresentare in modo generico un thread, ho inoltre definito delle strutture data per i thread lettori e scrittori (o decisono di usare una sola versione essendo che a parte per il puntatore al file ed il suo mutex queste due tipologie di thread hanno bisogno degli stessi dati, per il file ed il suo mutex semplicemente li mantendo a NULL, nel momento in cui non li uso) e per il gestore dei segnali