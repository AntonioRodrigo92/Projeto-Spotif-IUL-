#include "/home/so/trabalho-parte-2/spotifiul-media-player/spotifiul-media-player.h"
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "music.h"

int pid_graph, pid_lyric, pid_timer;
int n;
int i = 1;
int t;
int tgraph;
int tlyric;
char lyric[5000];
int musicDuration;
int ratio;
char nome[60], autor[50];
char *a[40];
musica lista[100];
int nr;

void lancar_musica (int i) {
    strcpy(nome, lista[i].name);
    strcpy(autor, lista[i].artist);
    musicDuration = lista[i].duration;
    strcpy(lyric, lista[i].lyric);
    t = 0;
    tlyric = 0;
    ratio = ((sizeof(lyric) / sizeof(char)) / musicDuration) + 1;
    printSongInfo(nome, autor, IUL_COLOR_WHITE);
    initProgress(musicDuration, IUL_COLOR_CYAN);
    initGraph();
    initLyrics();
}

void trata_alarme (int sinal) {
	t++;
	setProgress(t + 1);
	kill (pid_graph, SIGUSR1);
    kill (pid_lyric, SIGUSR1);
    if (n % 2 != 0) {
        alarm(1);
    }
	if (t == musicDuration) {
		kill(getppid(), SIGUSR1);
		exit(0);
	}
}

void trata_timer (int sinal) {
    ++n;
	signal (SIGALRM, trata_alarme);
    alarm(1);
}

void mata_timer (int signal) {
	setProgress(1);
    exit(0);
}

void mata_graph (int signal) {
	clearGraph();
    exit(0);
}

void mata_lyric (int signal) {
	clearLyrics();
    exit(0);
}

void trata_lyric (int signal) {
	++tlyric;
	printLyrics(IUL_COLOR_LIGHTGRAY, "%.*s", ratio, lyric + tlyric * ratio);
	if (tlyric == musicDuration) {
		exit(0);
	}
}

void trata_graph (int signal) {
	tgraph++;
	printRandomGraph();
	if (tgraph == musicDuration) {
        exit(0);
    }
}

void criar_filhos() {
	pid_lyric = fork();
	if (pid_lyric < 0) {
        perror ("Erro no fork");
        exit(1);
    }

    //filho: lyric
    if (pid_lyric == 0) {
		initLyrics();
		signal(SIGUSR1, trata_lyric);
        signal(SIGUSR2, mata_lyric);
	
        while (1) {
            pause();	
        }
     }

    //ELSE
    else {
        pid_graph = fork();
        if (pid_graph < 0) {
            perror ("Erro no fork");
            exit(1);
        }

        //filho: graph
        if (pid_graph == 0) {
			tgraph = 0;
			initGraph();
			signal(SIGUSR1, trata_graph);
            signal(SIGUSR2, mata_graph);

		    while (1) {
                pause();
            }
        }

		else {
            pid_timer = fork();
            if (pid_timer < 0) {
                perror ("Erro no fork");
                exit(1);
            }

            //filho: timer
            if (pid_timer == 0) {            
				initProgress(musicDuration, IUL_COLOR_CYAN);
                signal(SIGUSR1, trata_timer);
                signal(SIGUSR2, mata_timer);
 
				while(1) {
					pause();
				
                }
            }
		}
	}
}

void musica_acabou (int signal) {
	setProgress(0);
	closeProgress();
	clearGraph();
	closeGraph();
	clearLyrics();
	closeLyrics();
	if (i >= nr) {
		i = 1;
	}
	else {
		i++;
    }
    kill (pid_timer, SIGUSR2);
    kill (pid_graph, SIGUSR2);
    kill (pid_lyric, SIGUSR2);
    lancar_musica(i);
    criar_filhos();
}

int main() {
	char playlist [5000];
    char *aux1, *aux3;
    char aux2[40];
    musica temp;
	FILE *fl;
    int j = 1;

    printf("indique no nome da playlist: ");
    scanf("%s", &playlist);
    FILE *f;
    f = fopen ("playlists.txt", "r");
    if ( f == NULL) {
        perror ("file");
        exit(1);
    }
    aux1 = (char*)strtok(playlist,":");
    strcpy(aux2, aux1);
    char *a[40];
    while(  fgets( playlist ,5000 , f ) !=  NULL ){
        aux3 = (char*)playlist+sizeof (aux1)/sizeof(char)+sizeof(aux2)/sizeof(char);
	
	if( strstr( playlist, aux2) != NULL ){
            int contador = 0;

            while((aux3=strtok(NULL,":"))!=NULL){

                a[contador]=aux3;
                fl=fopen("lyrics.db","r");
                 if ( fl == NULL ){
                    perror("file");
                    exit(1);
                }
                 while(fread(&temp,sizeof(temp),1,fl)>0){


            if(strcmp(a[contador],temp.id)==0 || strcmp(a[contador],temp.id)==10){
                lista[j]=temp;
				j++;
                }
            }
            ++contador;
            }
        }
    }
    fclose ( f );
    fclose ( fl );
	nr = (j-1);

	startupSpotifIULMediaPlayer();
	showMediaPlayer(IUL_COLOR_YELLOW);

	lancar_musica(i);


	//pai: controlador
	criar_filhos();

	char s;
	while(1) {
		
		signal(SIGUSR1, musica_acabou);

		s = getch();
		if (s == ' ') {
			kill(pid_timer, SIGUSR1);
		}
		if (s == 'n') {
            if (i >= nr) {
                i = 1;
            }
			else {
				i++;
			}
            kill (pid_timer, SIGUSR2);
            kill (pid_graph, SIGUSR2);
            kill (pid_lyric, SIGUSR2);
            lancar_musica(i);
            criar_filhos();
		}
		if (s == 'p') {
            if(i <= 1){
                i = nr;
            }
			else {
				i--;
			}
            kill (pid_timer, SIGUSR2);
            kill (pid_graph, SIGUSR2);
            kill (pid_lyric, SIGUSR2);
            lancar_musica(i);
            criar_filhos();
        
		}
		if (s == '.') {
			kill (pid_timer, SIGUSR2);
			kill (pid_graph, SIGUSR2);
			kill (pid_lyric, SIGUSR2);
			lancar_musica(i);
			criar_filhos();
		}
		if (s == 'q') {
			kill (pid_timer, SIGUSR2);
			kill (pid_graph, SIGUSR2);
			kill (pid_lyric, SIGUSR2);
			closeProgress();
			closeGraph();
			closeLyrics();
			shutdownSpotifIULMediaPlayer();
			exit(1);
        }

	}   
}
