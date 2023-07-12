Info: progetto ridotto.

# Descrizione 'hash_map.c/.h'
In questo file ho voluto definire una struttura che contenesse tutti i dati che potevano servire per la gestione di una hash map, ho inotre definito le varie funzioni che operano su questa hash map.

## `hash_table_create()`
La funzione `hash_table_create` crea una nuova hash table. Questa funzione inizializza la hash table utilizzando la funzione `hcreate` della libreria `<search.h>`, alloca memoria per la struttura `hash_table_t`, imposta i valori iniziali dei suoi campi e restituisce un puntatore alla hash table creata.

### Parametri:
Nessun parametro.

### Valore di Ritorno:
Un puntatore a una nuova istanza della hash table (`hash_table_t`).

## `hash_table_insert(hash_table_t *hash_table, char *key)`
La funzione `hash_table_insert` inserisce una nuova entry nella hash table fornita come parametro. Se la chiave (`key`) non esiste nella hash table, viene creata una nuova entry e viene incrementato il contatore di occorrenze. Se la chiave esiste già, viene incrementato il contatore di occorrenze. La funzione supporta il threading e utilizza un mutex per garantire l'accesso esclusivo alla hash table durante l'inserimento.

### Parametri:
- `hash_table`: Un puntatore alla hash table (`hash_table_t`) in cui inserire la nuova entry.
- `key`: La chiave (`char*`) della nuova entry da inserire.

### Valore di Ritorno:
Nessun valore di ritorno.

## `hash_table_count(hash_table_t *hash_table, char *key)`
La funzione `hash_table_count` restituisce il numero di occorrenze della chiave specificata nella hash table fornita come parametro. Se la chiave non viene trovata nella hash table, viene restituito il valore 0. La funzione supporta il threading e utilizza un mutex per garantire l'accesso esclusivo alla hash table durante la ricerca.

### Parametri:
- `hash_table`: Un puntatore alla hash table (`hash_table_t`) in cui cercare la chiave.
- `key`: La chiave (`char*`) da cercare nella hash table.

### Valore di Ritorno:
Il numero di occorrenze della chiave nella hash table.

## `hash_table_destroy(hash_table_t *hash_table)`
La funzione `hash_table_destroy` libera la memoria allocata per la hash table e le relative entrys. Prima di deallocare la hash table, vengono deallocati anche i dati delle entrys. Infine, la funzione chiama `hdestroy` per liberare eventuali risorse allocate dalla funzione `hcreate`.

### Parametri:
- `hash_table`: Un puntatore alla hash table (`hash_table_t`) da distruggere.

### Valore di Ritorno:
Nessun valore di ritorno.


# Descrizione 'buffer.c/.h'
In questi file ho definito una struttura che rappresenta un buffer generico, quindi con mutex e condvar utili per la gestione della muta eslusione durante l'accesso al buffer da più thread. Ho inotre definito le funzione per operare in modo thread-safe sul buffer.

## `buffer_create()`
La funzione `buffer_create` crea un nuovo buffer. Questa funzione alloca memoria per la struttura `buffer_t`, inizializza i campi della struttura e restituisce un puntatore al buffer creato.

### Parametri:
Nessun parametro.

### Valore di Ritorno:
Un puntatore a una nuova istanza del buffer (`buffer_t`).

## `buffer_insert(buffer_t *buffer, char *str)`
La funzione `buffer_insert` inserisce una stringa nel buffer fornito come parametro. Se il buffer è pieno, la funzione attende fino a quando il buffer non diventa disponibile. La funzione supporta il threading e utilizza un mutex e una variabile di condizione per garantire l'accesso esclusivo al buffer durante l'inserimento.

### Parametri:
- `buffer`: Un puntatore al buffer (`buffer_t`) in cui inserire la stringa.
- `str`: La stringa (`char*`) da inserire nel buffer.

### Valore di Ritorno:
Nessun valore di ritorno.

## `buffer_remove(buffer_t *buffer)`
La funzione `buffer_remove` rimuove una stringa dal buffer fornito come parametro e la restituisce. Se il buffer è vuoto, la funzione attende fino a quando il buffer non contiene elementi. La funzione supporta il threading e utilizza un mutex e una variabile di condizione per garantire l'accesso esclusivo al buffer durante la rimozione.

### Parametri:
- `buffer`: Un puntatore al buffer (`buffer_t`) da cui rimuovere la stringa.

### Valore di Ritorno:
Un puntatore alla stringa (`char*`) rimossa dal buffer.

## `buffer_destroy(buffer_t *buffer)`
La funzione `buffer_destroy` libera la memoria allocata per il buffer. Prima di deallocare il buffer, vengono liberate anche le stringhe contenute al suo interno. Infine, la funzione chiama `pthread_mutex_destroy` e `pthread_cond_destroy` per distruggere i mutex e le variabili di condizione utilizzate per il threading.

### Parametri:
- `buffer`: Un puntatore al buffer (`buffer_t`) da distruggere.

### Valore di Ritorno:
Nessun valore di ritorno.

# Descrizione 'thread.c/.h'
In questo file ho definito una struttura che potesse rappresentare in modo generico un thread, ho inoltre definito delle strutture data per i thread lettori e scrittori (o decisono di usare una sola versione essendo che a parte per il puntatore al file ed il suo mutex queste due tipologie di thread hanno bisogno degli stessi dati, per il file ed il suo mutex semplicemente li mantendo a NULL, nel momento in cui non li uso) e per il gestore dei segnali

## `thread_create(void *data, void *(*tbody)(void *))`
La funzione `thread_create` crea un nuovo thread. Questa funzione alloca memoria per la struttura `thread_t`, imposta i campi della struttura con i dati forniti come argomenti e crea un nuovo thread utilizzando la funzione `pthread_create`. Il nuovo thread eseguirà la funzione `tbody` con il parametro `data`. La funzione restituisce un puntatore alla struttura del thread.

### Parametri:
- `data`: Il dato (`void*`) da passare alla funzione del thread.
- `tbody`: Il puntatore alla funzione (`void *(*tbody)(void *)`) che il thread eseguirà.

### Valore di Ritorno:
Un puntatore a una nuova istanza del thread (`thread_t`).

## `thread_destroy(thread_t *thread)`
La funzione `thread_destroy` libera la memoria allocata per la struttura del thread. Questa funzione deallocerà solo la struttura del thread, ma non terminerà il thread in esecuzione.

### Parametri:
- `thread`: Un puntatore alla struttura del thread (`thread_t`) da distruggere.

### Valore di Ritorno:
Nessun valore di ritorno.

# Descrizione 'connection.c/.h'
In questo file ho definito una struttura che rappresenta una connessione con due funzioni, una per creare e avviare la connessione ed una per stoppare e deallocare la struttura.

## `connection_create()`
La funzione `connection_create` crea una nuova connessione. Questa funzione alloca memoria per la struttura `connection_t`, crea un socket utilizzando la funzione `socket` con il dominio AF_INET e il tipo SOCK_STREAM, imposta i parametri dell'indirizzo del server nella struttura `serv_addr` e avvia la connessione utilizzando la funzione `connect`. La funzione restituisce un puntatore alla struttura della connessione.

### Parametri:
Nessun parametro.

### Valore di Ritorno:
Un puntatore a una nuova istanza della connessione (`connection_t`).

## `connection_destroy(connection_t *connection)`
La funzione `connection_destroy` libera la memoria allocata per la struttura della connessione e chiude il socket associato. La funzione utilizza la funzione `close` per chiudere il socket.

### Parametri:
- `connection`: Un puntatore alla struttura della connessione (`connection_t`) da distruggere.

### Valore di Ritorno:
Nessun valore di ritorno.