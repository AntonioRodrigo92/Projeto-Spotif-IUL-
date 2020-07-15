#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
void obter_substring (char *linha, char *id, char *nome,char *artista, char *duracao, char separador);

void sair(){
    printf("Byee!!\n");
    exit(0);
}

typedef struct {
	char id[10];
	char nome[60];
	char artista[50];
	int duracao;
	char letra[50000];
} Song;

int main(){
	printf("Importar musicas: Opcao 1\nLer: Opcao 2\nSair: Opcao 3\n");
	char opcao[2];
	scanf("%s",opcao);
    char dr[100];
	char aux[100], aux1[150]="\0", mus[60],art[50], line[256],id[10],nome[60],artista[50],duracao[10], letra[50000];
	char *result;//*id,*nome,*artista, *duracao;
	FILE *musica, *musicatxt;
	if((strcmp(opcao,"1")==0)){
		printf("Directoria ficheiros .lyric? \n" );
	    scanf("%s", dr);
		DIR *d;
		struct dirent *dir;
		d=opendir(dr);
		musicatxt=fopen("/home/so/trabalho-parte-2/musicas.txt","r");
		if(musicatxt==NULL)
        printf("Erro, nao foi possivel abrir musicas.txt\n");
		FILE *lyric;
		lyric=fopen("lyrics.db","wrb+");
		printf("%s\n",aux1);
  
	if(d){//se escolher importar faz
        while((dir=readdir(d))!=NULL){
			if(!strcmp(dir->d_name,".")) continue;
			if(!strcmp(dir->d_name,"..")) continue; 
			if( strstr(dir->d_name,".lyric")){          //abre
				strcpy(aux, dir->d_name);
				printf("%s\n", aux);
				strcat(aux1,dr);
				strcat(aux1,"/");
				strcat(aux1,aux);
			//	printf("%s\n",aux1);
				if((musica=fopen(aux1,"r"))==NULL){
					printf("erro");
					exit(1);
				}
				fgets(mus,60,musica);
				fgets(art,50,musica);
				sscanf(mus, "%[^\n]", mus);
				sscanf(art, "%[^\n]", art);

				memset(letra, 0, sizeof(letra));
				int max_size = sizeof(letra), size = 0;

				while(fgets(line,256,musica) != 0){
					strcat(letra, line);
				}

				rewind(musicatxt);
				while((fgets(line,256,musicatxt)!=NULL)){
						memset(id,0,sizeof(id));
                        memset(nome,0,sizeof(nome));
                        memset(artista,0,sizeof(artista));
                        memset(duracao,0,sizeof(duracao));
						strcpy(id, strtok(line,":"));
						strcpy(nome, strtok(NULL,":"));
						strcpy(artista, strtok(NULL,":"));
						strcpy(duracao, strtok(NULL,":"));
						
						printf("Checking for song %s on musicas.txt\n", nome);

						if((strcmp(mus,nome)==0)&& (strcmp(art,artista)==0)){
							rewind(lyric);
							int found = 0;
							Song s;

							 while( fread(&s, sizeof(Song), 1, lyric) && (found == 0)){
								printf("Is it this one? %s \n", s.id);
								if (strcmp(s.id, id) == 0) {
									//update
									found = 1;
									fseek(lyric,0,SEEK_SET-1*(int)(sizeof(s)));
									fwrite(s.letra,sizeof(Song),1,lyric); 
								}
							}

							if (found == 0) {
								printf("NOT FOUND, ADDING %s\n", id);
								strcpy(s.id, id);
								strcpy(s.nome,nome);
								strcpy(s.artista,artista);
								s.duracao = atoi(duracao);
								strcpy(s.letra,letra);
								fwrite(&s,sizeof(Song),1,lyric);
								
		//						size_t size = fread(&s, sizeof(Song), 1, lyric);
		//						printf("read %d byte", size);
		//						printf("THIS IS WHAT I READ %s\n\n\n\n", s.nome);
//exit(0);
								break;
							}
							break;
						}
				}
				memset(aux1,0,sizeof(aux1));
				sscanf(mus, "%[^\n]", mus);
				sscanf(art, "%[^\n]", art);
				printf("%s %s \n",mus,art);
			    fclose(musica);
			}
		}
    }
    closedir(d);
	fclose(musicatxt);
	fclose(lyric);
	}

	if ((strcmp(opcao,"2"))==0){
		printf("id da musica?\n");
		char id_introduzido[10];
		scanf("%s",id_introduzido);
		FILE *read=fopen("lyrics.db","rb");
		Song s;
		int found_id=0;
		 if(read==NULL){
            printf("ficheiro vazio");
            exit(1);
        }

		while( fread(&s, sizeof(Song), 1, read)) {
			//printf("entrou");
			if((strcmp(s.id,id_introduzido)==0))
				 printf("%s",s.letra);
			//else printf("Nao foi encontrado esse id");
			
		}
	}
	if ((strcmp(opcao,"3"))==0) 
		sair();
	//fclose(lyric);
}


void obter_substring (char *linha, char *id, char *nome,char *artista, char *duracao, char separador){
    int i,j=0,meu_indice=0;
    for(i=0; i!=strlen(linha); i++){
        if(*(linha+i)==separador){
			j=0;
            meu_indice++;}
        else if (meu_indice == 0){
            *(id+j++)=*(linha+i);}
        else if (meu_indice == 1){
            //j=0;
            //nome[j++]=linha[i];
			*(nome+j++)=*(linha+i);}
        else if (meu_indice == 2){
            //j=0;
           // artista[j++]=linha [i];
				*(artista+j++)=*(linha+i);}
        else if (meu_indice == 3){
           // j=0;
            //duracao[j++]=linha[i];
			*(duracao+j++)=*(linha+i);
        }
    }
}


