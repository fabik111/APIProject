#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include<string.h>
#define DIMHASH 3
//final version with re-hashing of the hashtable 
typedef struct hashtable_s{ //hashtable type 
	
	void *indirizzo;
}hashtable_t;
typedef struct elem_s{ //node type
	int8_t foc; //foc it's a sort of flag that identifies the type of elem: foc=0 -->empty folder foc=1-->folder with elements foc=3 -->file 
	uint8_t livello;//depth of the elem
	uint16_t countelem; //count of the elements in the folder
	char *nome; //name of the element
	char *contenuto;//if the element is a file this is the pointer to the written string
	hashtable_t *array;//if the element is a foldier this is the pointer of the hash table
	uint16_t dim_array;//dimension of the hashtable
	struct elem_s *prec;//I've used open hashing  so this is the pointer to the previous element in the list
	struct elem_s *next;//pointer to the following element 
}elem_t;
void create(char *, elem_t *root);//function of create command
void create_dir(char *, elem_t *root);//function of create_dir command
void read(char *, elem_t *root);//funciont of read command
void write_infile(char *, char *,elem_t * root);//function of write command
void del(char*, elem_t * root);//function of delete command 
void del_r(char *, elem_t * root);//function of delete_dir command
void find(char *,elem_t * root);//function of find command
elem_t *findpath(elem_t *root, char *arg, int *index,int *key, char *nome);//this function returns the pointer to the father element so it's possible insert a new element in the correct cell of the hash table
elem_t *create_root(void);//this function create the root folder
elem_t *searchinlist(elem_t * figlio, char*nome);//this function flows a list and return the pointer to the element that his name is equal to the argument nome
void free_subdir(elem_t*,bool);//this function deletes all elements in a folder 
void free_all(elem_t*);//this function clears all the elements in the "file system"
int compare(const void *a, const void *b);//compare function
char ** search(elem_t *,char *,char **array_string, int *,char *,int*);// this function helps the function find. It's recursive and returns the pointer to an array of strings. These strings are the path to the searched element  
char** aggiungi_string(char *string, int *indice, char **array_string, int *dim_array,char *nome); //this function puts the path of the found element in the array 
int hash_function(char *string, uint16_t dim_array);//hash function jenkins
hashtable_t *rehash(hashtable_t *old_array, uint16_t dim_array, elem_t * elem);//rehashing function
int main(int argc, char *argv)
{
    char *comando, *percorso, *argomento;
	char *stringa,c;
	elem_t *root;
	int i, spaziodisp;

	if (root=create_root())
	{
		
		do {
			stringa=(char*)malloc(50*sizeof(char));
		i=0;
		spaziodisp=50;
		do{
			if(i>=spaziodisp)
			{
				spaziodisp=spaziodisp+100;
				stringa=(char*)realloc(stringa, spaziodisp*sizeof(char));
			}
			c=getc(stdin);
			stringa[i]=c;
			i++;
		}while(c!=EOF && c!='\n');
		if((spaziodisp-i)<=0)
			stringa=(char*)realloc(stringa, (spaziodisp+1)*sizeof(char));
		stringa[i]='\0';
    	
		comando=strtok(stringa," ");
	   
    	if(strcmp(comando,"create")==0)
    	{
			percorso=strtok(NULL," ");
			create(percorso, root);
		}
		else if(strcmp(comando,"create_dir")==0)
		{
			percorso=strtok(NULL," ");
			create_dir(percorso,root);
		}
   		else if(strcmp(comando,"read")==0)
		{
			percorso=strtok(NULL," ");
			read(percorso,root);
		}
		else if(strcmp(comando,"write")==0)
		{
			percorso=strtok(NULL," ");
			argomento=strtok(NULL," ");
			write_infile(percorso, argomento,root);
		}
  		else if(strcmp(comando,"delete")==0)
		{
			percorso=strtok(NULL," ");
			del(percorso,root);
		}
	
    	else if(strcmp(comando,"exit\n")==0)
		{
		
			free(stringa);
			break;
		}
		else if(strcmp(comando,"delete_r")==0)
		{
			percorso=strtok(NULL," ");
			del_r(percorso,root);
		}
		else if(strcmp(comando,"find")==0)
		{
			percorso=strtok(NULL," ");
			find(percorso,root);
		}
		else
   			{
   				printf("stringa non riconosciuta\n");
			   free(stringa);
			   break;
			   }
			   
    	free(stringa);
    	
  		}while(true);
  	//printf("fine");
  	free_all(root);
  	}else{
  		printf("errore creazione root");
	  }
  	
	return 0;
}

  
void create(char *argomento, elem_t* root)
  {
  	elem_t *newelem, *padre, *tmp;
  	int index, key;
  	float collision, count_padre, dim_a_padre;
  	char *nome;
  	nome=(char *)malloc(10*sizeof(char));
	 if( padre=findpath(root,argomento,&index,&key,nome))
	 {   
	 	if(strlen(nome)>255){
	 		goto error_exit;
		 }
	 	if(tmp=padre->array[index].indirizzo)
	 		if(searchinlist(tmp,nome)!=NULL)
	 		{
			 printf("no\n");
			 free(nome);
			 return;
			}
		 if(padre->countelem<1024 && padre->livello<255)
	 	{
	 		if(newelem=malloc(sizeof(elem_t)))
	 		{
	 			 padre->countelem++;
				count_padre=padre->countelem;
				dim_a_padre=padre->dim_array;
				collision=count_padre/dim_a_padre;
				 if(collision>0.6){
				 padre->array=rehash(padre->array,padre->dim_array,padre);
				 index=hash_function(nome, padre->dim_array);	
				 }	 
				 padre->foc=1;
				 newelem->foc=3;
				 newelem->nome=nome;
				 newelem->countelem=0;
				 newelem->contenuto=NULL;
				 if(padre->array[index].indirizzo!=NULL){
				
			     for( tmp=padre->array[index].indirizzo; tmp->next; tmp=tmp->next);
				 
				 tmp->next=newelem;
				 newelem->prec=tmp;
				 newelem->next=NULL;
				 
				
				 }else{
				 
			     padre->array[index].indirizzo=newelem;
	 			 newelem->prec=padre;
	 			 newelem->next=NULL;
				 }
			 }else{
			 	goto error_exit;}
	 		
		 }
		 else
		 { 
		 goto error_exit;}
	 }
	 else{
	 	goto error_exit;}
	  
	  printf("ok\n");
	  //printf("create %s\n",argomento);
  	  
  	return;
  	error_exit:
  		free(nome);
  		printf("no\n");
		//printf("create %s\n",argomento);  		
return;
  }
void create_dir(char *argomento, elem_t * root)
 {
  	elem_t *newelem, *padre, *tmp;
  	int index, localkey;
  	float collision, count_padre, dim_a_padre;;
  	char *nome;
  	nome=(char *)calloc(10,sizeof(char));
  	
	 if( padre=findpath(root,argomento,&index,&localkey,nome))
	 {
	 	if(strlen(nome)>255){
	 		goto error_exit;
		 }
		 if(tmp=padre->array[index].indirizzo)
	 		if(searchinlist(tmp,nome)!=NULL)
	 		goto end;
		 if(padre->countelem<1024 && padre->livello<255)
	 	{
	 		if(newelem=malloc(sizeof(elem_t)))
	 		{
	 			 padre->countelem++;
	 			 count_padre=padre->countelem;
	 			 dim_a_padre=padre->dim_array;
				  collision=count_padre/dim_a_padre;
				 if(collision>0.6){
				 padre->array=rehash(padre->array,padre->dim_array,padre);
				 index=hash_function(nome, padre->dim_array);	
				 }
	 			 padre->foc=1;
				 newelem->foc=0;
				 newelem->nome=nome;
				 newelem->countelem=0;
				 newelem->livello=padre->livello++;
				 newelem->dim_array=DIMHASH; 
				 newelem->array=(hashtable_t*)calloc(DIMHASH,sizeof(hashtable_t));
				 if(newelem->array==NULL)
				 {
				 	printf("errore allocazione array in create_dir\n");
					free(newelem);				 	
					goto error_exit;
				 }
				 memset(newelem->array,NULL,DIMHASH);
				 if(padre->array[index].indirizzo!=NULL){
				
			     for( tmp=padre->array[index].indirizzo; tmp->next; tmp=tmp->next);
				 tmp->next=newelem;
				 newelem->prec=tmp;
				 newelem->next=NULL;
				 
				 }else{
				padre->array[index].indirizzo=newelem;
	 			newelem->prec=padre;
	 			newelem->next=NULL;
				 }
			 }else{
			 	goto error_exit;}	
		 }
		 else
		 {
		 goto error_exit;}
	 }
	 else{
	 	goto error_exit;}
	end:	
	printf("ok\n");
	//printf("create_dir %s\n",argomento);
	return;	
	error_exit:
		printf("no\n");
		//printf("create_dir %s\n",argomento);		
free(nome);
		return;
		
 }
void read(char *argomento, elem_t *root)
 {
	elem_t *padre,*file, *tmp, *figlio;
	int index, localkey;
	char *nome;
	nome=(char *)malloc(10*sizeof(char));
	if(padre=findpath(root,argomento,&index,&localkey,nome))
	{ 
	   if(padre->array[index].indirizzo!=NULL){ 
	
	   	figlio=padre->array[index].indirizzo;
	   
	   
	   	if(figlio->next==NULL){
	   		if(strcmp(figlio->nome,nome)==0)
	   		  {
	   			file=figlio;
			   }else{
			   	goto error_exit;
			   }
	   		
		   }else if(figlio->next!=NULL)
		   {
		   	 file=searchinlist(figlio,nome);
			 if(file==NULL){
			  goto error_exit;
			 }
			}    
	  	 }else
	  	 {
	  	 	printf("file non trovato\n");
	  	 	goto error_exit;}
 	}else
	{
		goto error_exit;
	}
	if(file->foc!=3){
		printf("non file\n");
		goto error_exit;}
    if (file->contenuto==NULL)
	{
	printf("contenuto \n");

	}
	else 
	printf("contenuto %s\n", file->contenuto);
	//printf("read %s\n",argomento);
	free(nome);
	return;
	error_exit:
		printf("no\n");
		//printf("read %s\n",argomento);		
		free(nome);		
		return;
}
void write_infile(char *percorso, char *stringa,elem_t *root)
 {
	elem_t *padre,*file, *figlio;
	char *contenuto;
	int index, localkey, m,n;
	char *nome;
	nome=(char *)malloc(10*sizeof(char));
	if(padre=findpath(root,percorso,&index,&localkey,nome))
	{ 
		if(padre->array[index].indirizzo!=NULL){
		
	   	figlio=padre->array[index].indirizzo;
	   	if(figlio->next==NULL){
	   		if(strcmp(figlio->nome,nome)==0)
	   		{
	   			file=figlio;
			   }else{
			   		goto error_exit;}	
		   }else if(figlio->next!=NULL ){
		      file=searchinlist(figlio,nome);
		   	  if(file==NULL)
		   	  {
		   	  	goto error_exit;}
		   }   
        }else{
        	goto error_exit;}
	}else
	{
		goto error_exit;}
	if(file->foc!=3){
		printf("non file\n");
		goto error_exit;}
	if(contenuto=(char*)malloc(strlen(stringa)*sizeof(char)))	{
	for(n=0,m=1; stringa[m]!='"';m++,n++)
    contenuto[n]=stringa[m];
    contenuto[n]='\0';
	if(file->contenuto!=NULL)
	  free(file->contenuto);
	file->contenuto=contenuto;
	printf("ok %d\n",n);
	
   }else{
   	printf("no\n");
   }
free(nome);
  //printf("write %s %s\n",percorso, stringa);
return;
 error_exit:
 	printf("no\n");
		//printf("write %s %s\n",percorso, stringa);
		free(nome);
		return;
}
void del(char *argomento, elem_t *root)
{
	elem_t *padre,*del, *tmp, *figlio;
	int index, localkey, elem_padre,index_cache;
	char *nome;
	nome=(char *)malloc(10*sizeof(char));
	if(padre=findpath(root,argomento,&index,&localkey,nome))
	{ 
	   if(padre->array[index].indirizzo!=NULL){ 
	
	   	figlio=padre->array[index].indirizzo;
		
		if(strcmp(figlio->nome,nome)==0)
	   	{
				  	
				del=figlio;
	   			if(del->foc==1){
	   				printf("elemento non cancellabile\n");
	   				free(nome);
					printf("no\n");
	   				return;
				   }
			   if(figlio->next==NULL){
			
			   padre->array[index].indirizzo=NULL;
		       
		           
			   }
		       else
		       {
		       	padre->array[index].indirizzo=figlio->next;
			    tmp=figlio->next;
			    tmp->prec=padre;
			   			  
 }
			}
		  else if(figlio->next!=NULL){
					   	
			del=searchinlist(figlio,nome);
					   	
			if(del==NULL)
		   	{
		   	goto error_exit;
			   }
			if(del->foc==1){
	   				printf("elemento non cancellabile\n");
	   				goto error_exit;}
			
			tmp=del->prec;
		   	tmp->next=del->next;
		   	if(del->next!=NULL){
		   		tmp=del->next;
		   		tmp->prec=del->prec;
			   }
					   
			}
		else 
		   {
		   	printf("file non trovato\n");
		   	goto error_exit; }
	  	}else
	  	 {
	  	  goto error_exit;}
        
	}else
	{
		goto error_exit;}
	if(del->foc==0){
					    	
				free(del->array);
			    	free(del->nome);
		                free(del);
			    	
				}else if(del->foc==3){
					free(del->contenuto);
					free(del->nome);
					free(del);		
				}
	 
	 elem_padre=padre->countelem; 
	 elem_padre--;
	 padre->countelem=elem_padre;
	 if(padre->countelem==0)
	 padre->foc=0;
			   
	printf("ok\n");
	//printf("del %s\n",argomento);
	free(nome);
	return;
	error_exit:
		printf("no\n");
		//printf("del %s\n",argomento);		
		free(nome);
		return;
}
void del_r(char *argomento,elem_t *root)
{
	elem_t *padre,*del, *figlio, *tmp;
	char *contenuto;
	int index, localkey, index_cache;
	char *nome;
	nome=(char *)malloc(10*sizeof(char));
	if(padre=findpath(root,argomento,&index,&localkey,nome))
	{ 
	   if(padre->array[index].indirizzo!=NULL){ 
	   	figlio=padre->array[index].indirizzo;
	   	 if(strcmp(figlio->nome,nome)==0)
	   	{
	   			del=figlio;
			   if(figlio->next==NULL){
			 
			   padre->array[index].indirizzo=NULL;
		      
			   }
		       else
		       {
		       	padre->array[index].indirizzo=figlio->next;
			    tmp=figlio->next;
			    tmp->prec=padre;
			   }
			
	   		
		   }else if(figlio->next!=NULL ){
		   	del=searchinlist(figlio,nome);
		   	tmp=del->prec;
		   	tmp->next=del->next;
		   	if(del->next!=NULL){
		   		tmp=del->next;
		   		tmp->prec=del->prec;
		   }
		   else 
		   {
		   	//printf("file non trovato\n");
		   	goto error_exit;
		   }
	  	 }else
	  	 {
	  	 	//printf("errore key\n");
	  	 	goto error_exit;}
        }else{
        	goto error_exit;}
	}else
	{
	goto error_exit;}
	free_subdir(del,false);
	 padre->countelem--;
	 if(padre->countelem==0)
	 padre->foc=0;
	printf("ok\n");
	//printf("del_r %s\n",argomento);
	free(nome);
	return;
	error_exit:
			printf("no\n");
		//printf("del_r %s\n",argomento);		
		free(nome);
		return;
}
void find(char * argomento, elem_t *root)
{
	char *string;
	char **array_string=malloc(20*sizeof(char *));
	char **array_finale;
	int dim_argomento, dim_array,indice,i;
	indice=0;
	string=(char *)malloc(2*sizeof(char));
	string[0]='\0';
	dim_array=20;
	dim_argomento=strlen(argomento);
	argomento[dim_argomento-1]='\0';
array_finale=search(root,string,array_string,&indice,argomento,&dim_array);
	
	qsort(array_finale, indice, sizeof(char *), compare);
	if(indice==0)
	{
		printf("no\n");
		goto end;		
	}
	for(i=0;i<indice;i++)
	{
		printf("ok %s\n",array_finale[i]);
	}

	for(i=0;i<indice; i++)
	{
		free(array_finale[i]);
	}
	end:	
	free(array_finale);
	free(string);
	//printf("find %s\n",argomento);
	return;
}
char **search(elem_t *elem, char *string, char **array_string, int *indice, char * cercato, int *dim_array)
{
	int i,dim_string, dim_tmp, dim_hashtable, indice_array;
	char *tmp;
	elem_t *indappoggio;
	dim_string=strlen(string)+1;
	tmp=(char *)malloc(dim_string*sizeof(char));
	strcpy(tmp,string);
							
	if(strcmp(elem->nome,cercato)==0)
		{
						
			array_string=aggiungi_string(string,indice,array_string, dim_array,elem->nome );
		
			*indice=*indice+1;
			if(elem->foc==3)
			{
			free(tmp);
			return array_string;
			}
		}
	 
	if(elem->next!=NULL)
	{
		array_string=search(elem->next,tmp,array_string,indice,cercato,dim_array);
		
		}	
	dim_tmp=dim_string+strlen(elem->nome)+1;
	tmp=(char *)realloc(tmp,dim_tmp*sizeof(char));
	strcat(tmp,elem->nome);
	if(strcmp(elem->nome,"/")!=0)
		strcat(tmp,"/");  
	dim_hashtable=elem->dim_array;
	if(elem->foc!=3)
	for(i=0; i<dim_hashtable;i++)
	{
		if(elem->array[i].indirizzo!=NULL)
    		{
    			indappoggio=elem->array[i].indirizzo;
				array_string=search(indappoggio,tmp,array_string,indice,cercato,dim_array);
			}
	}
	free(tmp);
	return array_string;
}
char **aggiungi_string(char *string, int *indice, char **array_string, int *dim_array, char* nome)
{
	char *inser;
	int index,k,h;
  k=*indice;
  h=*dim_array;
	if(*indice>(*dim_array-1))
	{
		*dim_array=*dim_array+1;
		array_string=(char **)realloc(array_string,*dim_array*sizeof(char *));
	}
	index=*indice;
	inser=(char * )malloc((strlen(string)+strlen(nome)+1)*sizeof(char));
	strcpy(inser,string);
	strcat(inser,nome);
	array_string[index]=inser;
	
	return array_string;
}

elem_t *searchinlist(elem_t * figlio, char*nome)
{
	elem_t *tmp;
	for(tmp=figlio; tmp;tmp=tmp->next)
		   	{
		   		
		   		if(strcmp(tmp->nome,nome)==0)
		   		{
		   			return tmp;
				   }
			   }
	return NULL;
}
void free_subdir(elem_t* del, bool del_lista)
{
	elem_t *tmp;
	int i,foc, dim_array;
	
	//if che controlla se del->next sia valido e se del_lista sia settato a true in queste condizioni allora mando free_subdir(del->next) cos\EC cancello prima tutti gli elementi della lista prima di proseguirecon glialtri elementi in del->array
	if(del->next!=NULL && del_lista==true)
	{
		free_subdir(del->next,true);
	}
	foc=del->foc;
	free(del->nome);
	if(foc==0){
		free(del->array);		
		
	}
	if(foc==3)
	{
	    free(del->contenuto);
			}
    
	if(foc==1)
    {
    	dim_array=del->dim_array;
    	for(i=0; i<dim_array;i++)
    	{
    		if(del->array[i].indirizzo!=NULL)
    		{	
				tmp=del->array[i].indirizzo;
				free_subdir(tmp,true);
			}
		}
		free(del->array);
		}
	free(del);
return;
}
void free_all(elem_t *root)
{
	//free(root->nome);
	free_subdir(root,true);
	//free(root->array);
	//free(root);
	return;
}
elem_t *create_root(void)
{
	elem_t *root;
	if(root=(elem_t *)malloc(sizeof(elem_t)))
	{
		root->nome=(char *)malloc(2*sizeof(char));
		root->nome[0]='/';
		root->nome[1]='\0';
		root->foc=0;
		root->countelem=0;
		root->livello=0;
		root->array=(hashtable_t*)calloc(DIMHASH,sizeof(hashtable_t));
		memset(root->array,NULL,DIMHASH);
		root->dim_array=DIMHASH;
		root->next=NULL;
		root->prec=NULL;
		
		return root;
	}
	return NULL;
}
int compare(const void *a, const void *b){
    char **a1= (char **)a;
    char **b1= (char **)b; 
    return strcmp(*a1,*b1);
}
elem_t *findpath(elem_t *root, char *arg, int *index,int *key, char *nome)
{
	char separatore[] = "/";
	char * primo;
	char *secondo;
	char  *string;
	int dist, dim_nome;
	elem_t *padre;
	primo=strpbrk(arg, separatore);
	 padre=root;
	 while (primo != NULL)
  {
    secondo=strpbrk (primo+1,separatore);
    if(secondo==NULL)
    	break;
	dist=(int)(secondo-primo);
	string=(char *)malloc(dist*sizeof(char));
    if(string==NULL)
    	return NULL;
	strncpy(string,primo+1,dist-1);
	string[dist-1]='\0';
    *index=hash_function(string, padre->dim_array);
      //printf("%d\n",padre->dim_array);
    padre=padre->array[*index].indirizzo;
    if(padre==NULL)
    {
    	printf("si sta tentando di accedere a un elemento o non presente o non corretto\n");
    	
		free(string);
		return NULL;
	}
	if(strcmp(padre->nome,string)!=0)
	{
		if(padre->next!=NULL)
		{
			
			padre=searchinlist(padre,string);
			if(padre==NULL)
			{
		    free(string);	
			return NULL;
	     	} 
		
		}else
		{   free(string);
			return NULL;
		}
		
	}
	if(padre->foc==3)
	{
		return NULL;
	}
	primo=secondo;
    free(string);
  }
	dim_nome=strlen(primo);
	if(dim_nome>10)
	{
		nome=(char *)realloc(nome,dim_nome*sizeof(char));
	}
	
	if(primo[dim_nome-1]=='\n'){
	strncpy(nome,primo+1,dim_nome-2);
	nome[dim_nome-2]='\0';
    }else{
    	strcpy(nome,primo+1);
	}
	*index=hash_function(nome,padre->dim_array); 
	return padre;
}
int hash_function(char *string, uint16_t dim_array)
{
	int i,length,val_ret, key;
	i=0;
	length=strlen(string);
  int hash = 0;
  while (i != length) {
    hash += (int)string[i];
    hash += hash << 10;
    hash ^= hash >> 6;
  	i++;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  hash=abs(hash);
  key=hash;
  val_ret=hash%dim_array;
  return val_ret;
}
hashtable_t *rehash(hashtable_t *old_array, uint16_t dim_array, elem_t * elem)
{
	hashtable_t *new_array;
	elem_t *tmp, *curr_elem, *indappoggio, *next;
	int i,k,m, old_dim_array, new_dim_array;
	int index;
	old_dim_array=dim_array;
	new_dim_array=dim_array+11;
	elem->dim_array=new_dim_array;
	new_array=(hashtable_t *)calloc(new_dim_array,sizeof(hashtable_t));
	memset(new_array,NULL,new_dim_array);
	for(i=0; i<old_dim_array; i++ ){
		curr_elem=old_array[i].indirizzo;
		if(curr_elem!=NULL){
		if(curr_elem->next==NULL)
		{
		index=hash_function(curr_elem->nome,new_dim_array);
		if(new_array[index].indirizzo!=NULL){
			for( tmp=new_array[index].indirizzo; tmp->next; tmp=tmp->next);
				 
				 tmp->next=curr_elem;
				 curr_elem->prec=tmp;
				 curr_elem->next=NULL;
		}
		else{
			new_array[index].indirizzo=curr_elem;
	 			 curr_elem->prec=elem;
	 			 curr_elem->next=NULL;
		}
		
	}else{
		for(indappoggio=curr_elem; indappoggio; indappoggio=next){
			index=hash_function(indappoggio->nome,new_dim_array);
		if(new_array[index].indirizzo!=NULL){
			for( tmp=new_array[index].indirizzo; tmp->next; tmp=tmp->next);
				 
				 tmp->next=indappoggio;
				 indappoggio->prec=tmp;
				 next=indappoggio->next;
				 indappoggio->next=NULL;
		 }
		else{
			new_array[index].indirizzo=indappoggio;
	 			 indappoggio->prec=elem;
	 			 next=indappoggio->next;
				  indappoggio->next=NULL;
		  }
		}
	 }
   }
 }
 free(old_array);
 return new_array;
}
