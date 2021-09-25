#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>



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

/*STRUCTURE DEFINITON: flags*/
struct serdata{
  char l_chr;
  int err_flag;
  int arr_len;
  int amp_flag;
  int gts_flag;
  int d_gts_flag;
  int lts_flag;
  struct charlist *gts_name;
  struct charlist *lts_name;
  struct charlist *d_gts_name;
};

/*CONDITION FUNCTION: spacebar(quotation) exit status checker*/
int spbex(char chr,int qm_marker)
{
  return ((chr != ' ')||(qm_marker % 2  != 0));
}

/*CONDITION FUNCTION: EOL exit status checker*/
int eolex(char chr)
{
  return (chr != '\n');
}

/*CONDITION FUNCTION: EOF exit status checker*/
int eofex(char chr)
{
  return (chr != EOF);
}

/*CONDITION FUNCTION: Composition of previously declared functions*/
int ex_cond(char c,int q)
{
  return (spbex(c,q)&&eolex(c)&&eofex(c));
}

/*FUNCTION: comparing 2 strings*/
int str_comp(char* str1, char* str2)
{
  while((*str1 != '\0')&&(*str2 != '\0')){
    if( *str1 != *str2  ){
      return 0;
    }
    str1++;
    str2++;
  }
  return *str1 == *str2;
}

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

/*FUNCTION: deleting base array*/
void del_barray(char** barr,int len)
{
  int i;
  for(i=0;i<len;i++)
  {
    free(barr[i]);
  }
  free(barr);
}

/*FUNCTION: deleting only baselist segments and keeping  words unfreed*/
void partial_del_baselist(struct baselist *bas)
{
  struct baselist *tmp;
  while(bas){
    tmp=bas->next;
    free(bas);
    bas=tmp;
  }
  bas=NULL;
}

/*FUNCTION: full base list and words deletion (in case of QM ERROR)*/
void full_del_baselist(struct baselist *bas)
{
  struct baselist *tmp;
  while(bas){
    tmp=bas->next;
    free(bas->word);
    free(bas);
    bas=tmp;
  }
  bas=NULL;
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

/*FUNCTION: list size*/
int base_list_size(struct baselist *chr)
{
  int i=0;
  struct baselist *tmp;
  tmp=chr;
  while(tmp){
    i++;
    tmp=tmp->next;
  }
  return i;
}

/*FUNCTION: list to array*/
char* lta(struct charlist *chr)
{
  int i,ls=list_size(chr);
  char *a;
  struct charlist *tmp;
  ls++;
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
  }else{
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
  }else {
    while (tmp->next != NULL){
      tmp=tmp->next;
    }
    tmp->next=malloc(sizeof(struct baselist));
    tmp->next->word=a;
    tmp->next->next=NULL;
    return fel;
  }
}

/*FUNCTION: Getting filename*/
void get_file_name(struct serdata *f,struct charlist** name,char* c)
{
  while((*c == ' ')||(*c == '>')||(*c == '<'))
  {
    *c=getchar();
  }
  while((*c != '\n')&&(*c != ' ')){
    *name = add_block_temp(*name, *c);
    *c = getchar();
    if(*c == '&'){
      f->amp_flag=1;	
      *c = getchar();
    }			
  }
}

/*FUNCTION: Reading string*/
struct charlist* read_string(struct serdata* flags)
{
  int qm_marker=0;
  char chr = flags->l_chr;
  struct charlist *fel=NULL;
  while(ex_cond(chr,qm_marker)){
    switch(chr){
      case '"':
        qm_marker++;
        chr=getchar();
        break;
      case '>':
        chr = getchar();
        if(chr == '>'){
          get_file_name(flags,&flags->d_gts_name,&chr);
          flags->d_gts_flag++;
          break;
        }else{
          get_file_name(flags,&flags->gts_name,&chr);
          flags->gts_flag++;
          break;
        }
      case '&':
        flags->amp_flag++;
        chr=getchar();
        break;
      case '<':
        get_file_name(flags,&flags->lts_name,&chr);
        flags->lts_flag++;
        break;
      default:
        if(flags->amp_flag>0)
          flags->amp_flag=2;
        fel=add_block_temp(fel,chr);
        chr=getchar();
    }
  }
  if(qm_marker % 2 == 1){
    del_charlist(fel);
    flags->l_chr = '"';
    fel=NULL;
    return fel;
  }else{
    flags->l_chr=chr;
  }
  return fel;
}

/*FUNCTION: Building the temporary list*/
struct charlist* build_list(struct serdata* flags)
{
  char chr;
  do
  {
    chr=getchar();
    if(chr == EOF){
      flags->l_chr=EOF;
      return NULL;
    }
  }while(chr == ' ');
  flags->l_chr=chr;
  return read_string(flags); 
}

/*FUNCTION: base list to base array*/
char** blist_to_barray(struct baselist *fel,struct serdata* f)
{
  char **barr;
  struct baselist* tmp=fel;
  int len=base_list_size(fel),i=0;
  barr=malloc(sizeof(char*)*(len+1));
  for(i=0;i<len;i++)
  {
    barr[i]=tmp->word;
    tmp=tmp->next;
  }
  barr[i]=NULL;
  f->arr_len=len;
  partial_del_baselist(fel);
  fel=NULL;
  return barr;
}

/*FUNCTION: executing in terminal*/
void execute(char** barr, struct serdata* f)
{
  int t,fd=0;
  if( barr[0] == NULL)
  return;
  if ( str_comp(barr[0], "cd") &&  barr[1] != NULL) {
    t = chdir(barr[1]);
    if (t == -1 ){
      perror(barr[1]);
    }
  }else
  {
    pid_t pid;
    pid = fork();
    switch(pid) {
      case -1:
        perror("Fork:");
        break;
      case 0:
        if(f->gts_flag == 1){
          fd=open(lta(f->gts_name),O_WRONLY|O_CREAT|O_TRUNC,0666);
          dup2(fd,1);
        }else{
          if(f->d_gts_flag == 1){
            fd=open(lta(f->d_gts_name),O_WRONLY|O_CREAT|O_APPEND,0666);
            dup2(fd,1);
          }
        }
        if(f->lts_flag == 1){
          fd = open(lta(f->lts_name),O_RDONLY,0666);
          if(fd == -1){
            perror("Input file error:");
            exit(1);
          }else
            dup2(fd,0);
        }
        execvp(barr[0],barr);
        perror(barr[0]);
        exit(0);
        break;
      default:
        if(f->amp_flag == 0){
          while(wait(NULL) != pid){
          }
        }
    }
  }
}


/*FUNCTION: DECLARING STRUCTURE*/
void reset_struct(struct serdata* flags)
{
  flags->err_flag=0;
  flags->l_chr=0;
  flags->arr_len=0;
  flags->amp_flag=0;
  flags->gts_flag=0;
  flags->lts_flag=0;
  flags->d_gts_flag=0;
  flags->gts_name=NULL;
  flags->lts_name=NULL;
  flags->d_gts_name=NULL;
}

/*FUNCTION: last char handler*/
void lc_handler(struct serdata* f,struct baselist* fel)
{
  char ** barr;
  switch(f->l_chr){
    case '"':
      printf("Error: quotation marks unbalaced\n");
      full_del_baselist(fel);
      break;
    default:
      if((f->gts_flag>1)||(f->d_gts_flag>1)){
        printf("Error: too many redirection outputs\n");
        full_del_baselist(fel);
        break;
      }
      if(f->lts_flag>1){
        printf("Error: too many redirection inputs\n");
        full_del_baselist(fel);
        break;
      }
      if(f->l_chr != EOF){
        barr=blist_to_barray(fel,f);
        execute(barr, f);
        del_barray(barr,f->arr_len+1);
        break;
      }
  }
}


/*MAIN FUNCTION*/
int main()
{
  char *a;
  struct serdata *f = malloc(sizeof(struct serdata));
  struct baselist *fel=NULL;
  struct charlist *cfel=NULL;
  do
  {
    reset_struct(f);
    fel=NULL;
    printf(">");
    while((f->l_chr!='\n')&&(f->l_chr!='"')&&(f->l_chr!=EOF)){
      cfel=build_list(f);
      if(cfel != NULL){
        a=lta(cfel);
        fel=add_block_base(fel, a);
      }
    }
    if(f->amp_flag > 1){
      full_del_baselist(fel);
      f->amp_flag=0;
      printf("Error: Ampersand position invalid \n");
    }else
      lc_handler(f, fel);
    if(f->gts_name != NULL){
      del_charlist(f->gts_name);
    }
    if(f->d_gts_name != NULL){
      del_charlist(f->d_gts_name);
    }
    if(f->lts_name != NULL){
      del_charlist(f->lts_name);
    }
  }while(f->l_chr != EOF);
  return 0;
}

