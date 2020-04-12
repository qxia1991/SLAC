/* 
This program generates macro files to run the EXO executable.
To compile: gcc generate_exo.c -o generate_exo
*/

#include <stdio.h>

#define NCOM 100
#define MAXCOMMAND 120

 
main()

{

  char c;
  char filename[100];
  char ifilename[100];
  char inputfilename[100];
  char inextension[100];
  char outputfiledirectory[200];
  int njobs;
  double  dnevents;
  int nevents;
  int monte_carlo;
  int outputfile;
  int root_or_binary;
  int icom;
  int ichar;


  char command[NCOM*MAXCOMMAND];


  FILE *fptr;
  int i, j;
 
  printf("Enter the output .exo file name (without the .exo)\n");
  scanf("%s",filename);

  njobs = 0;
  printf("Enter the number of parallel jobs to create\n");
  scanf( "%d", &njobs);
  while ( njobs <=0 || njobs > 10000 ) {
    scanf( "%d", &njobs);
  }

  monte_carlo = 0;
  printf("Are you generating monte carlo events? (y or n)\n");
  c = getchar();
  while ( c != 'y' && c != 'n' ) {
    c = getchar();
  }
  if ( c == 'y' ) {
    monte_carlo = 1;
    printf("Enter the total number of events to generate\n");
    scanf("%le", &dnevents);
    nevents = (int)(dnevents/njobs);
    printf("will generate %d events per job\n",nevents);
  }
  else {
    printf("Enter the input file name\n");
    scanf("%s",inputfilename);
    printf("Enter the input file extension (including the '.'\n");
    scanf("%s",inextension);
  }

  printf("Enter any commands you wish to appear in the output file\n");
  printf("For example: use exosim rec toutput\n");
  printf("No need to give an output file command.\n"); 
  printf("Enter Control-D to finish.\n");

  icom = 0;
  ichar = 0;
  c = getchar();
  while ( c != EOF ) {
    if ( c != '\n' ) {
      command[icom*MAXCOMMAND+ichar] = c;
      ichar++;
    }
    else {
      command[icom*MAXCOMMAND + ichar] = '\0';
      /*      printf("read in command: %s\n", &command[icom*MAXCOMMAND]);*/
      icom++;
      ichar = 0;
    }   
    c = getchar();
    if ( icom >= NCOM ) break;
    if ( ichar >= MAXCOMMAND ) {
      printf("command too long!\n");
      break;
    }
  }

  outputfile = 0;
  root_or_binary = -1;
  printf("Would you like to add an output file command? (y or n)\n");
  c = getchar();
  while ( c != 'y' && c != 'n' ) {
    c = getchar();
  }
  if ( c == 'y' ) {
    outputfile = 1;
    printf("output root file or binary file? (0 for root, 1 for binary)\n");
    scanf("%d",&root_or_binary);
    while ( root_or_binary < 0 || root_or_binary > 1 ) scanf("%d",&root_or_binary);
    printf("Enter the output file directory (with trailing backslash)\n");
    scanf("%s",outputfiledirectory);
  }

  for ( i = 0; i < njobs; i++ ) {

    if ( njobs == 1 ) {
      sprintf(ifilename,"%s.exo",filename);
    }
    else {
      sprintf(ifilename,"%s_%d.exo",filename,i);
    }
      
    if (  ( fptr = fopen( ifilename, "w" ) ) == NULL ) {
      printf("can't open output file: %s\n", filename );
      return;
    }

    for ( j = 0; j < icom; j++ ) {
      fprintf(fptr,"%s\n", command + j*MAXCOMMAND);
    }

    if ( monte_carlo == 1 ) {
      fprintf(fptr,"/exosim/initial_seed %d\n",i);
      fprintf(fptr,"maxevents %d\n", nevents );
    }
    else {
      if ( njobs == 1 ) {
	fprintf(fptr, "/input/file %s%s\n", inputfilename, 
		inextension);
      }
      else {
	fprintf(fptr, "/input/file %s_%d%s\n", inputfilename, 
		i, inextension);
      }
    }

    if ( outputfile == 1 ) {
      if ( njobs == 1 ) { 
	if ( root_or_binary == 0 ) {
	  fprintf(fptr, "/toutput/file %s%s.root\n", 
		  outputfiledirectory, filename );
	}
	else {
	  fprintf(fptr, "/boutput/file %s%s.bin\n", 
		  outputfiledirectory, filename );
	}
      }
      else {
	if ( root_or_binary == 0 ) {
	  fprintf(fptr, "/toutput/file %s%s_%d.root\n", 
		  outputfiledirectory, filename, i );
	}
	else {
	  fprintf(fptr, "/boutput/file %s%s_%d.bin\n", 
		  outputfiledirectory, filename, i );
	}
      }

    }

    fprintf(fptr, "begin\n");
    fprintf(fptr, "exit\n");
    fclose(fptr);
    

  }

}

