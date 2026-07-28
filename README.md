# Progetto Basi di Dati - Olimpiadi

Questo repository contiene il progetto realizzato per il corso di Basi di Dati. Il sistema modella la gestione dei Giochi Olimpici, permettendo di gestire e interrogare informazioni riguardanti atleti, nazioni, discipline, gare e risultati.

Il progetto è composto da un database relazionale e da un applicativo client scritto in linguaggio C.

## Struttura del Repository

La directory principale contiene i seguenti file:

* **`Olimpiadi/`**
  * **`Olimpiadi.sql`**: Script SQL contenente il codice DDL per la creazione dello schema del database (tabelle, vincoli, chiavi esterne) e il codice DML per il popolamento iniziale dei dati.
  * **`Olimpiadi.c`**: Codice sorgente dell'applicativo client in C. Si interfaccia con il database per eseguire query, inserimenti e operazioni gestionali.
* **`relazioneprogetto.pdf`**: Documentazione ufficiale del progetto. Contiene l'analisi dei requisiti, lo schema Concettuale (Modello E/R), lo schema Logico, la normalizzazione e le scelte progettuali adottate.

## Requisiti di Sistema

Per testare ed eseguire il progetto è necessario avere installato sul proprio computer:

*   Un DBMS relazionale (es. PostgreSQL o MySQL).
*   Un compilatore C (es. `gcc`).
*   La libreria di sviluppo C per il database scelto (es. `libpq-dev` per PostgreSQL o `libmysqlclient-dev` per MySQL).

## Installazione e Configurazione

### 1. Creazione del Database
Prima di avviare l'applicativo, è necessario creare il database e popolarlo utilizzando lo script SQL fornito.

*Esempio con PostgreSQL:*
```bash
# Accedi a psql e crea il database
CREATE DATABASE olimpiadi;

# Esci e importa lo script
psql -U tuo_username -d olimpiadi -f Olimpiadi/Olimpiadi.sql
