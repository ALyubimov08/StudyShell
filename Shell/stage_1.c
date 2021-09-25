#include <stdio.h>
#include <stdlib.h>

/*STRUCTURE DEFINITION: temporary list*/
struct charlist{
  char c;
  struct charlist *next;
};

/*STRUCTUTE DEFINITION: the base list*/
struct baselist{
  struct baselist *next;
  char *word;
};

/*FUNCTION: temporary list deletion*/
void del_charlist(struct charlist *chr)
{
  struct charlist *tmp;
  while(chr){
    tmp=chr->next;
    free(chr);
    chr=tmp;
  }
};

/*FUNCTION: base list and words deletion*/
void del_baselist(struct baselist *bas)
{
  struct baselist *tmp;
  while(bas){
    tmp=bas->next;
    free(bas->word);
    free(bas);
    bas=tmp;
  }
  bas=NULL;
};

/*FUNCTION: base list elements output*/
void out_b_list(struct baselist *p)
{
  struct baselist *tmp;
  tmp=p;
  while(tmp){
    printf("%s",tmp->word);
    putchar(' ');
    tmp=tmp->next;
  }
  printf("\n");
  del_baselist(p);
}

/*FUNCTION: list size*/
int list_size(struct charlist *chr)
{
  int i=0;
  struct charlist *tmp;
  tmp=chr;
  while(tmp){
    i++;
    tmp=tmp->next;
  }
  return i;
}

/*FUNCTION: list to array*/
char* list_to_arr(struct charlist *chr)
{
  int i,ls=list_size(chr);
  ls++;
  char *a;
  struct charlist *tmp;
  tmp=chr;
  a=malloc(ls);
  for(i=0; i<ls-1; i++){
    a[i]=tmp->c;
    tmp=tmp->next;
  }
  a[i]='\0';
  del_charlist(chr);
  return a;
}

/*FUNCTION: Adding block to temporary list*/
struct charlist* add_block_temp(struct charlist *fel,char chr)
{
  struct charlist *tmp=NULL;
  tmp=fel;
  if (tmp == NULL){
    tmp=malloc(sizeof(struct charlist));
    tmp->c=chr;
    tmp->next=NULL;
    return tmp;
  }else
  {
    while (tmp->next != NULL){
      tmp=tmp->next;
    }
    tmp->next=malloc(sizeof(struct charlist));
    tmp->next->c=chr;
    tmp->next->next=NULL;
    return fel;
  }
}

/*FUNCTION: Adding block to temporary list*/
struct baselist* add_block_base(struct baselist *fel,char *a)
{
  struct baselist *tmp=NULL;
  tmp=fel;
  if (tmp == NULL){
    tmp=malloc(sizeof(struct baselist));
    tmp->word=a;
    tmp->next=NULL;
    return tmp;
  }else
  {
    while (tmp->next != NULL){
      tmp=tmp->next;
    }
    tmp->next=malloc(sizeof(struct baselist));
    tmp->next->word=a;
    tmp->next->next=NULL;
    return fel;
  }
}


/*FUNCTION: Building the temporary list*/
struct charlist* build_list(char *last_char)
{
  char chr;
  struct charlist *fel=NULL;
  int qm_marker=0;
  do
  {
    chr=getchar();
    if(chr == EOF){
      *last_char=EOF;
      return NULL;
    }
  }while(chr == ' ');
  while((chr!=EOF)&&(chr!= '\n')&&((chr != ' ')||(qm_marker % 2  != 0))){
    if(chr == '"'){
      qm_marker++;
      chr=getchar();
    }else
    {
      fel=add_block_temp(fel, chr);
      chr=getchar();
    }
  }
  if(qm_marker % 2 == 1){
    del_charlist(fel);
    *last_char = '"' ;
    fel=NULL;
    return fel;
  }else
  {
    *last_char=chr;
    return fel;
  }
}

/*MAIN FUNCTION*/
int main()
{
  char *a;
  char last_chr;
  struct baselist *fel=NULL;
  struct charlist *cfel;
  do
	{
    last_chr=0;
    while((last_chr != '\n')&&(last_chr != '"')&&(last_chr != EOF)){
      cfel=build_list(&last_chr);
      if(cfel != NULL){
        a=list_to_arr(cfel);
        fel=add_block_base(fel, a);
      }
    }
    if(last_chr == '"'){
      printf("ERROR: quotation marks unbalaced \n");
      del_baselist(fel);
      fel=NULL;
    }else
    {
      if(last_chr != EOF){
        out_b_list(fel);
        fel=NULL;
      }
    }
	}while(last_chr != EOF);
	return 0;
}
