/*
 * buffer.c : generate an executable to test the sockets.
 * it is used to create/test the sent/receive messages
 *
 *  $Id$
 */

#include <stdlib.h>
#include "buffer.h"

typedef unsigned char BOOLEAN;           
typedef BOOLEAN *PBOOLEAN;       

#define TRUE				1
#define FALSE				0
#define SIZE_INTERVAL		50
#define CHAR_BUFFER			'0'
#define CHAR_LIMIT			'L'
#define	CHAR_END			'E'

static  unsigned long s_BufferId_ul	= 0;
static  unsigned long s_BufferId_ul_rcv	= 1;

static int Length = 10;
static int koYes = 0;
static int frameAfterKo = 5;

unsigned char * BuildBuffer() {
  unsigned char *	Buffer_puc;
  unsigned long *	Buffer_pul;
  int			i;
  int			SizeDataBuffer_i;
  

  /* Seed the random-number generator with current time so that
   * the numbers will be different every time we run.
   */
  /*srand( (unsigned int)(s_BufferId_ul*time(NULL)));*/
  
  Length++;
  if (Length > 2000)
    Length = 10;
	
  Buffer_puc = (unsigned char *)malloc(Length);
  if( Buffer_puc == NULL )
    printf( "Insufficient memory available\n" );

  /* Build the buffer 
   * The first word = The length of the buffer
   * Every SIZE_INTERVAL char write the CHAR_LIMIT
   * The last cahr is CHAR_END
   * The other char is CHAR_BUFFER */
  
  
  /* Intialize the length of the buffer in the first word */
  Buffer_pul = (unsigned long *)Buffer_puc;
  *Buffer_pul = Length;

  /* Intialize the length of the buffer in the first word*/
  Buffer_pul++;
  s_BufferId_ul++;
  *Buffer_pul = s_BufferId_ul;

  /* Intialize the buffer with the CHAR_LIMIT and CHAR_BUFFER */
  SizeDataBuffer_i = Length-8;
  Buffer_puc+=8;
  
  for (i= 0; i<SizeDataBuffer_i; i++){
    if( (i%SIZE_INTERVAL)==0 ) 
      Buffer_puc[i] = CHAR_LIMIT;
    else 
      Buffer_puc[i] = CHAR_BUFFER;
  }  

  Buffer_puc[SizeDataBuffer_i-2] = CHAR_END;
  Buffer_puc[SizeDataBuffer_i-1] = '\0';

  Buffer_puc-=8;
  
  printf("\nBuffer sent ID=<%ld> and length=<%d>/<0x%x>\n", s_BufferId_ul, Length, Length);
  fflush(stdout);
  
  return Buffer_puc;

}


void FreeBuffer(unsigned char * p_Buffer_puc){
  free(p_Buffer_puc);	
}


unsigned char * AllocBuffer()
{
  unsigned char *	Buffer_puc;

  Buffer_puc = (unsigned char *)malloc(SIZE_MAX_RCV_BUFFER);

  if( Buffer_puc == NULL ){
    printf("Insufficient memory available\n");
    fflush(stdout);
  }
  return Buffer_puc;	
}



unsigned long CheckBuffer(unsigned char * p_Buffer_puc)
{
	
  BOOLEAN	       	CheckBuffer_b = TRUE;
  unsigned long *	Buffer_pul;
  unsigned long 	LenBuffer_ul;
  int				i=0;
  unsigned long		BufferId_ul, j;


  /*Retrieve the length of the buffer*/
  Buffer_pul = (unsigned long *)p_Buffer_puc;
  LenBuffer_ul = *Buffer_pul;

  /* Retrieve the Id of the buffer*/
  Buffer_pul++;
  BufferId_ul = *Buffer_pul;

  if (BufferId_ul < s_BufferId_ul_rcv) {
    s_BufferId_ul_rcv = BufferId_ul;
  }
	
  if (BufferId_ul > s_BufferId_ul_rcv) {
    for (j = 0; j < (BufferId_ul - s_BufferId_ul_rcv); j++) {
      printf("!!!!!!!! Buffer ID=<%ld> lost !!!!!!!!\n", s_BufferId_ul_rcv);
      fflush(stdout);
      s_BufferId_ul_rcv++ ;
    }
    /* error */
    koYes = 0; /* if koYes = 1 here, the prog will stop 5 reception after */
  }

  s_BufferId_ul_rcv++ ;
  
  /* Print the information*/ 
  printf("Buffer receive ID=<%ld> and length=<%ld>/<0x%lx> :",BufferId_ul,LenBuffer_ul,LenBuffer_ul);
  fflush(stdout);
  
  /* Check the end character*/
  if(p_Buffer_puc[LenBuffer_ul-2]!=CHAR_END){
    CheckBuffer_b = FALSE;
  }

  /* Check the sequence characters*/
  p_Buffer_puc+=8;
  
  while((CheckBuffer_b)&&(i<(int)(LenBuffer_ul-10)))
    {
      
      if( (i%SIZE_INTERVAL)==0 ) 
	CheckBuffer_b = (p_Buffer_puc[i]==CHAR_LIMIT);
      else
	CheckBuffer_b = (p_Buffer_puc[i]==CHAR_BUFFER);
      
      i++;
    }
  
  p_Buffer_puc-=8;
  
  if (CheckBuffer_b) {
    printf("OK\n");
    fflush(stdout);
  }
  else {
    /* error */
    koYes = 0;	/* if koYes = 1 here, the prog will stop 5 receptions after */
    printf("KO ???\n");
    fflush(stdout);
  }
  
  if (koYes){
    frameAfterKo-- ;
    if (frameAfterKo == 0)
      return 0;
  }
  
  return BufferId_ul;
  
}	






