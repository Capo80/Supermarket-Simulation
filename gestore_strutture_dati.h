//
// Created by ezio on 19/08/20.
//

#include "strutture_dati.h"
#include <stdlib.h>
#include <stdio.h>


//-------------------------------------------------------
//funzioni per la gestione dei clienti
//-------------------------------------------------------

//genera il numero di oggetti secondo un esponenziale
//a media.. vedere relazione
int get_num_oggetti(){

    //TODO chiamare esponenziale con media campionaria della relazione

    return 5;
}

//un cliente è generato a partire da un evento di tipo arrivo,
//per la generazione è passata l'ora associato all'evento di arrivo.
struct cliente* genera_cliente(int ora_evento_arrivo){

    struct cliente *cli = (struct cliente *)malloc(sizeof(struct cliente));
    cli->num_oggetti = get_num_oggetti();
    cli->in_fila = ora_evento_arrivo;

    return cli;
}


//-------------------------------------------------------
//funzione per la gestione delle file (fila_cassa)
//-------------------------------------------------------

struct fila_cassa *crea_fila(){

    struct fila_cassa *fc = (struct fila_cassa *)malloc(sizeof(struct fila_cassa));
    fc->cliente_in_fila = NULL;

    return fc;
}

int aggiungi_cliente_infila(struct fila_cassa *fila, struct cliente *cli){

    fila->cliente_in_fila = cli;
    fila->next = (struct fila_cassa *)malloc(sizeof(struct fila_cassa));
    fila->next->cliente_in_fila = NULL;

    return 0;
}

int lunghezza_fila(struct fila_cassa *fila){

    int l = 0;

    for(struct fila_cassa *f = fila; f->cliente_in_fila != NULL; f = f->next ){
        l++;
    }

    return l;
}

void rimuovi_cliente_infila(struct fila_cassa *fila){
    //necessario ?
}


//-------------------------------------------------------
//funzione per la gestione delle casse
//-------------------------------------------------------

//ritorna delle casse, ovvero un determinato numero di file
struct casse *genera_set_casse(int num_casse){

    struct casse *casse = (struct casse *)malloc(sizeof(struct casse));

    //crea le file per ogni cassa
    int i = 0;
    struct casse *c;
    for(c = casse; ;c = c->next) {
        c->fila_cassa = crea_fila();
        i++;
        if (i < num_casse) {
            c->next = (struct casse *) malloc(sizeof(struct casse));
        }else{
            c->next = NULL;
            break;
        }

    }

    return casse;
}

int aggiungi_cassa(struct casse *casse){

    struct casse *c = casse;
    while(1){

        if(c->next == NULL){
            c->next = genera_set_casse(1);
            break;
        }
    }

    casse->fila_cassa = crea_fila();
    casse->next = (struct casse *)malloc(sizeof(struct casse));

    return 0;
}

int numero_casse(struct casse *casse){

    int l = 0;
    for(struct casse *c = casse; c != NULL; c = c->next ){
        l++;
    }
    return l;
}

void rimuovi_cassa(struct casse *casse){
    //necessario ?
}


//-------------------------------------------------------
//funzione per la gestione delle configurazioni di cassa
//-------------------------------------------------------

//aggiunge una configurazione di un dato tipo alle configurazioni attive (VARIABILE GLOBALE in costanti.h)
//casse_casuali_in_mista rappresenta quante casse delle 'numero_casse' sono dedicate alla
//configurazione pseudo casuale, il resto andranno alla selettiva.
//TODO aggiungere alla relazione ?
//usando tale funzione possiamo 'eliminare la configurazione di cassa mista', in quanto basterebbe aggiungere
//una configurazione pseudocasuale e poi una selettiva. Potremmo addirittura fare configurazioni
//non esposte nella relazione, come pseudocasuale + selettiva, pseudo casuale + random.....
int aggiungi_configurazione_cassa(int tipo, int numero_casse, int casse_casuali_in_mista){

    if((tipo != condivisa && tipo != pseudo_casuale && tipo != selettiva && tipo != mista) || numero_casse == 0){
        printf("Input errati\n");
        return -1;
    }

    if( tipo == mista ){

        if(numero_casse-casse_casuali_in_mista <= 0){
            printf("Input errati\n");
            return -1;
        }

        aggiungi_configurazione_cassa(pseudo_casuale, casse_casuali_in_mista, -1);
        aggiungi_configurazione_cassa(selettiva, numero_casse-casse_casuali_in_mista, -1);

        return 0;

    }

    struct config_cassa_attive *current = config_attive;

    if(current != NULL) {
        do{
            current = current->next;
        }while(current != NULL);
    }

    current = (struct config_cassa_attive *)malloc(sizeof(struct config_cassa_attive));
    current->next = NULL;
    current->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
    struct config_cassa *nuova_configurazione = current->configurazione_cassa;

    int legg = 0, med = 0, pes = 0;

    switch(tipo){
        case condivisa:

            nuova_configurazione->tipo = condivisa;
            nuova_configurazione->casse = genera_set_casse(numero_casse);
            nuova_configurazione->fila_condivisa = NULL;

            break;
        case pseudo_casuale:

            //gestire poi in modo che ogni cassa generata gestisca un cliente alla volta,
            //prelevandolo dalla fila condivisa
            nuova_configurazione->tipo = condivisa;
            nuova_configurazione->casse = genera_set_casse(numero_casse);
            nuova_configurazione->fila_condivisa = crea_fila();

            break;

        case selettiva:
            //incremento in modo prioridatio dalla cassa leggera alla pesante
            //il numero di casse date loro a disposizione
            do{
                legg++;
                if(numero_casse-legg-med-pes == 0){
                    break;
                }
                med++;
                if(numero_casse-legg-med-pes == 0){
                    break;
                }
                pes++;
                if(numero_casse-legg-med-pes == 0) {
                    break;
                }
            }while(1);

            nuova_configurazione->tipo = selettiva_leggera;
            nuova_configurazione->casse = genera_set_casse(legg);
            nuova_configurazione->fila_condivisa = NULL;

            if(med > 0) {
                current->next = (struct config_cassa_attive *)malloc(sizeof(struct config_cassa_attive));
                current = current->next;

                current->next = NULL;
                current->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
                nuova_configurazione = current->configurazione_cassa;

                nuova_configurazione->tipo = selettiva_media;
                nuova_configurazione->casse = genera_set_casse(med);
                nuova_configurazione->fila_condivisa = NULL;
            }

            if(pes > 0){
                current->next = (struct config_cassa_attive *)malloc(sizeof(struct config_cassa_attive));
                current = current->next;

                current->next = NULL;
                current->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
                nuova_configurazione = current->configurazione_cassa;

                nuova_configurazione->tipo = selettiva_pesante;
                nuova_configurazione->casse = genera_set_casse(pes);
                nuova_configurazione->fila_condivisa = NULL;
            }
            break;

    }

    return 0;

}


//ESPERIMENTO, se non funge, eliminare
//Funzione che permette di introdurre una cassa selettiva con specifiche a grana fina, ovvero (eg.) possibilità di inserire
//3 casse leggere con una fila condivisa e 2 pesanti con file separate ecc....
//qui ci si può sbizzarrire nella fase di sperimentazione
void aggiungi_configurazione_selettiva_custom(int num_casse_leggere, int num_casse_medie, int num_casse_pesanti,
                                                int fila_leggera_condivisa, int fila_media_condivisa, int fila_pesante_condivisa){

    struct config_cassa_attive *current = config_attive;

    if(current != NULL) {
        do{
            current = current->next;
        }while(current != NULL);
    }

    current = (struct config_cassa_attive *)malloc(sizeof(struct config_cassa_attive));
    current->next = NULL;
    current->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
    struct config_cassa *nuova_configurazione = current->configurazione_cassa;

    if(num_casse_leggere != 0){

        nuova_configurazione->tipo = selettiva_leggera;
        nuova_configurazione->casse = genera_set_casse(num_casse_leggere);
        if(fila_leggera_condivisa) {
            nuova_configurazione->fila_condivisa = crea_fila();
        }else {
            nuova_configurazione->fila_condivisa = NULL;
        }
    }

    if(num_casse_medie != 0){
        current->next = (struct config_cassa_attive *)malloc(sizeof(struct config_cassa_attive));
        current = current->next;

        current->next = NULL;
        current->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
        nuova_configurazione = current->configurazione_cassa;

        nuova_configurazione->tipo = selettiva_media;
        nuova_configurazione->casse = genera_set_casse(num_casse_medie);
        if(fila_media_condivisa) {
            nuova_configurazione->fila_condivisa = crea_fila();
        }else {
            nuova_configurazione->fila_condivisa = NULL;
        }
    }

    if(num_casse_pesanti != 0){
        current->next = (struct config_cassa_attive *)malloc(sizeof(struct config_cassa_attive));
        current = current->next;

        current->next = NULL;
        current->configurazione_cassa = (struct config_cassa *) malloc(sizeof(struct config_cassa));
        nuova_configurazione = current->configurazione_cassa;

        nuova_configurazione->tipo = selettiva_pesante;
        nuova_configurazione->casse = genera_set_casse(num_casse_pesanti);
        if(fila_pesante_condivisa) {
            nuova_configurazione->fila_condivisa = crea_fila();
        }else {
            nuova_configurazione->fila_condivisa = NULL;
        }
    }

}



//-------------------------------------------------------
//funzione per la gestione degli eventi
//-------------------------------------------------------

int aggiungi_evento(int tipo, int ora_evento, struct fila_cassa *fila){

    struct lista_eventi *current = eventi;

    struct evento *nuovo_evento =(struct evento *)malloc(sizeof(struct evento));
    nuovo_evento->tipo = tipo;
    if(tipo == servito){
        nuovo_evento->fila = fila;
    }
    nuovo_evento->tempo = ora_evento;

    //se la lista di eventi è vuota, mettilo per primo.
    if(eventi == NULL){
        eventi = (struct lista_eventi *)malloc(sizeof(struct lista_eventi));
        eventi->evento = nuovo_evento;
        eventi->next = NULL;
        eventi->prev = NULL;

        return 0;
    }

    //altrimenti cerca dove metterlo.
    struct lista_eventi *new_el = (struct lista_eventi *)malloc(sizeof(struct lista_eventi));
    new_el->next = NULL;
    new_el->prev = NULL;
    new_el->evento = nuovo_evento;

    do{

        if(current->evento->tempo < ora_evento){
            current = current->next;
            if(current->next == NULL){
                current->next = new_el;
                new_el->prev = current;

                return 0;
            }else{
                continue;
            }
        }else{
            //caso in cui lo devo inserire in testa.
            if(current->prev == NULL){
                new_el->next = current;
                current->prev = new_el;
                eventi = new_el;

                return 0;
            //caso in cui lo devo inserire in mezzo.
            }else{
                current->prev->next = new_el;
                new_el->next = current;
                current->prev = new_el;
                new_el->prev = current->prev;

                return 0;
            }
        }
    }while(1);

}

//gerera ora di arrivo, usato per generare a monte tutti gli arrivi dei clienti,
//e dunque tutti gli eventi di tipo 'arrivo'. Ciascun evento sarà associato ad un orario
//che verrà usato per la creazione del cliente.
int genera_ora_arrivo(int ora){

    int media_arr = 0;

    if(ore_6 < ora && ora <= ore_7){
        media_arr = arrivi_6_7;

    }else if(ore_7 < ora && ora <= ore_8){
        media_arr = arrivi_7_8;

    }else if(ore_8 < ora && ora <= ore_9){
        media_arr = arrivi_8_9;

    }else if(ore_9 < ora && ora <= ore_10){
        media_arr = arrivi_9_10;

    }else if(ore_10 < ora && ora <= ore_11){
        media_arr = arrivi_10_11;

    }else if(ore_11 < ora && ora <= ore_12){
        media_arr = arrivi_11_12;

    }else if(ore_12 < ora && ora <= ore_13){
        media_arr = arrivi_12_13;

    }else if(ore_13 < ora && ora <= ore_14){
        media_arr = arrivi_13_14;

    }else if(ore_14 < ora && ora <= ore_15){
        media_arr = arrivi_14_15;

    }else if(ore_15 < ora && ora <= ore_16){
        media_arr = arrivi_15_16;

    }else if(ore_16 < ora && ora <= ore_17){
        media_arr = arrivi_16_17;

    }else if(ore_17 < ora && ora <= ore_18){
        media_arr = arrivi_17_18;

    }else if(ore_18 < ora && ora <= ore_19){
        media_arr = arrivi_18_19;

    }else if(ore_19 < ora && ora <= ore_20){
        media_arr = arrivi_19_20;

    }else if(ore_20 < ora && ora <= ore_21){
        media_arr = arrivi_20_21;

    }else if(ore_21 < ora && ora <= ore_22){
        media_arr = arrivi_21_22;

    }

    //TODO chiamare esponenziale con media_arr

    return 30;
}

void genera_evento_servito(struct cliente *c, struct fila_cassa *fila){

    int n = c->num_oggetti;
    //TODO chiamare esponenziale in base al numero di oggetto del cliente
    int tempo_di_servizio = 60;


    //TODO qui
    aggiungi_evento(servito, tempo_di_servizio + c->iniziato_a_servire , fila);
}





//-------------------------------------------------------
//funzione per la stampa degli eventi e configurazioni di cassa attive
//-------------------------------------------------------


char *tipo_config(int tipo){

    switch(tipo){
        case selettiva:
            return selettiva_str;
        case pseudo_casuale:
            return pseudo_casuale_str;
        case condivisa:
            return condivisa_str;
        default:
            return "Non Registrata";
    }

}

void info_su_configurazioni_attive(){

    int i = 1;
    for(struct config_cassa_attive *current = config_attive; current != NULL; current = current->next){
        printf("Configurazioni attive:\n");
        printf("%d]-%s\n", i, tipo_config(current->configurazione_cassa->tipo));

        printf("Fila condivisa: ");
        if(current->configurazione_cassa->fila_condivisa == NULL) {
            printf("NO\n");
        }else{
            printf("SI\n");
            printf("Clienti in fila condivisa: %d\n", lunghezza_fila(current->configurazione_cassa->fila_condivisa));
        }

        int nc = numero_casse(current->configurazione_cassa->casse);
        printf("Numero di casse: %d\n", nc);

        int j = 0;
        for(struct casse *fc = current->configurazione_cassa->casse; fc != NULL; fc = fc->next){
            j++;
            printf("Clienti in fila alla cassa '%d': %d\n", j, lunghezza_fila(fc->fila_cassa));
        }

    }
}




















