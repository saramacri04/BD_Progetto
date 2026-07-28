#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

void check_connection(PGconn *conn) {
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
}

void esegui_query(PGconn *conn, const char *query, const char *param_prompt, int n_params) {
    const char *params[1];
    char input[100];

    if (n_params == 1) {
        printf("%s", param_prompt);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // rimuovi newline
        params[0] = input;
    }

    PGresult *res = PQexecParams(conn,
                                 query,
                                 n_params,
                                 NULL,
                                 (n_params == 1 ? params : NULL),
                                 NULL,
                                 NULL,
                                 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Errore nella query: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int n_fields = PQnfields(res);
    int n_rows = PQntuples(res);

    for (int i = 0; i < n_fields; i++) {
        printf("%-25s", PQfname(res, i));
    }
    printf("\n");

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_fields; j++) {
            printf("%-25s", PQgetvalue(res, i, j));
        }
        printf("\n");
    }

    PQclear(res);
}

int main() {
    PGconn *conn = PQconnectdb("dbname=olimpiadi user=postgres password=postgres");
    check_connection(conn);

    int scelta;
    do {
        printf("\nMenu:\n");
        printf("1. Partite tra squadre dello stesso continente\n");
        printf("2. Numero giocatori infortunati per squadra (per genere)\n");
        printf("3. Top 6 squadre per altezza media\n");
        printf("4. Squadre con almeno X punti nei set\n");
        printf("5. Nazioni con giocatori MVP\n");
        printf("0. Esci\nScelta: ");
        scanf("%d", &scelta);
        getchar(); // Consuma newline

        switch (scelta) {
            case 1:
                esegui_query(conn,
                    "SELECT P.Data, N1.Nome AS Nazione1, N2.Nome AS Nazione2, N1.Continente, P.Genere, P.Fase "
                    "FROM Partita P "
                    "JOIN Nazione N1 ON P.Squadra1 = N1.Codice_ISO "
                    "JOIN Nazione N2 ON P.Squadra2 = N2.Codice_ISO "
                    "WHERE N1.Continente = N2.Continente AND N1.Continente = $1 "
                    "ORDER BY P.Data;",
                    "Inserisci continente (es. Europa): ", 1);
                break;

            case 2:
                esegui_query(conn,
                    "SELECT N.Nome AS Nome_Nazione, S.Genere, COUNT(DISTINCT I.Giocatore) AS Numero_Giocatori_Infortunati "
                    "FROM Squadra S "
                    "JOIN Nazione N ON S.Codice_ISO = N.Codice_ISO "
                    "JOIN Giocatore G ON G.Squadra = S.Codice_ISO AND G.Genere = S.Genere "
                    "JOIN Infortunio I ON I.Giocatore = G.Codice_Fiscale "
                    "WHERE S.Genere = $1 "
                    "GROUP BY N.Nome, S.Genere;",
                    "Inserisci genere (M/F): ", 1);
                break;

            case 3:
                esegui_query(conn,
                    "SELECT N.Nome AS Nome_Nazione, S.Genere, ROUND(AVG(G.Altezza), 1) AS Media_Altezza "
                    "FROM Giocatore G "
                    "JOIN Persona P ON G.Codice_Fiscale = P.Codice_Fiscale "
                    "JOIN Squadra S ON P.Codice_ISO = S.Codice_ISO AND P.Genere = S.Genere "
                    "JOIN Nazione N ON S.Codice_ISO = N.Codice_ISO "
                    "GROUP BY N.Nome, S.Genere "
                    "ORDER BY Media_Altezza DESC "
                    "LIMIT 6;",
                    NULL, 0);
                break;

            case 4:
                esegui_query(conn,
                    "SELECT S.Codice_ISO, N.Nome AS Nome_Nazione, SUM(Punti) AS Totale_Punti "
                    "FROM ( "
                    "  SELECT Squadra1 AS Codice_ISO, Punteggio_1 AS Punti FROM Set "
                    "  UNION ALL "
                    "  SELECT Squadra2 AS Codice_ISO, Punteggio_2 AS Punti FROM Set "
                    ") AS PuntiPerSquadra "
                    "JOIN Squadra S ON PuntiPerSquadra.Codice_ISO = S.Codice_ISO "
                    "JOIN Nazione N ON S.Codice_ISO = N.Codice_ISO "
                    "GROUP BY S.Codice_ISO, N.Nome "
                    "HAVING SUM(Punti) >= $1 "
                    "ORDER BY Totale_Punti DESC;",
                    "Inserisci soglia punti minimi (es. 1000): ", 1);
                break;

            case 5:
                esegui_query(conn,
                    "SELECT n.Nome AS Nazione, COUNT(*) AS Numero_MVP "
                    "FROM Statistica s "
                    "JOIN Giocatore g ON s.MVP = g.Codice_Fiscale "
                    "JOIN Persona p ON g.Codice_Fiscale = p.Codice_Fiscale "
                    "JOIN Nazione n ON p.Codice_ISO = n.Codice_ISO "
                    "GROUP BY n.Nome "
                    "ORDER BY Numero_MVP DESC;",
                    NULL, 0);
                break;

            case 0:
                printf("Uscita...\n");
                break;

            default:
                printf("Scelta non valida.\n");
        }
    } while (scelta != 0);

    PQfinish(conn);
    return 0;
} 