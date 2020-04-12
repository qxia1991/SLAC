/* 
This program generates macro files to run the EXO executable.
To compile: gcc generate_mac.c -o generate_mac
*/

#include <stdio.h>

  char filename[100];
  char ifilename[100];
  int njobs;
  float wirenoise;
  float APDnoise;
  float energy_res;
  float digitime;
  char c;
  int lxe_only;
  int calc_signals;
  int igen;
  int ivolume;
  int zion, aion, zmin, zmax, amin, amax;
  double  dnevents;
  int nevents;
  int print_modulo;
  float xenon_absl;
  float xenon_rayleigh;
  float xenon_scint_yield;
  float alpha_energy;
  float fano_factor;
 
main()

{

  void make_mac_files();
  char c;
  char filename_save[100];
  int nevents_save;
  double save_digitime;
 
  printf("Enter the output mac file name\n");
  scanf("%s",filename);

  printf("Use default value of 800 e- RMS for electronic noise on wires? (y or n)\n");
  c = getchar();
  while ( c != 'y' && c != 'n' ) {
    c = getchar();
  }
  if ( c == 'y' ) {
    wirenoise = 800.0;
  }
  else {
    printf("Enter the RMS electrons of noise on wires\n");
    scanf( "%f", &wirenoise);
  }

  printf("Use default value of 2000 e- RMS for electronic noise on gang-of-seven APDs? (y or n)\n");
  c = getchar();
  while ( c != 'y' && c != 'n' ) {
    c = getchar();
  }
  if ( c == 'y' ) {
    APDnoise = 2000.0;
  }
  else {
    printf("Enter the RMS electrons of noise on gang-of-seven APDs\n");
    scanf( "%f", &APDnoise);
  }


  printf("Use default value of 1.5 percent for LXe energy resolution? (y or n)\n");
  c = getchar();
  while ( c != 'y' && c != 'n' ) {
    c = getchar();
  }
  if ( c == 'y' ) {
    energy_res = 0.015;
  }
  else {
    printf("Enter the LXe energy resolution in percent\n");
    scanf( "%f", &energy_res);
    energy_res = energy_res*0.01;
  }
  
  printf("Keep only LXe events? (y or n)\n");
  c = getchar();
  while ( c != 'y' && c != 'n' ) {
    c = getchar();
  }
  if ( c == 'y' ) {
    lxe_only = 1;
  }
  else {
    lxe_only = 0;
  }

  printf("Calculate wire and APD signals? (y or n)\n");
  c = getchar();
  while ( c != 'y' && c != 'n' ) {
    c = getchar();
  }
  if ( c == 'y' ) {

    printf("By default, digitization time will be 2048 microseconds\n");
    printf("for Bi-214 decays and 512 microseconds for all other events.\n");
    printf("Is this OK?\n");

    c = getchar();
    while ( c != 'y' && c != 'n' ) {
      c = getchar();
    }
    if ( c == 'y' ) {
      digitime = 0.0;
    }
    else {
      printf("Enter the total digitization time in microseconds\n");
      scanf( "%f", &digitime);
    }

    calc_signals = 1;

  }
  else {

    calc_signals = 0;

  }

  igen = 0;
  printf("Choose generator:\n");
  printf("\t1 = bb0n\n");
  printf("\t2 = bb2n\n");
  printf("\t3 = radioactive ion\n");
  printf("\t5 = Radon 222 decay chain\n");
  printf("\t6 = Uranium decay chain\n");
  printf("\t7 = Radon 220 decay chain\n");
  printf("\t8 = Thorium decay chain\n");
  printf("\t9 = Alpha particle\n");
  scanf("%d",&igen);

  if ( igen < 1 || igen > 9 || igen == 4 ) {
    printf("generator unknown\n");
    return;
  }

  if ( igen > 2 ) {

    printf("Enter the decay volume:\n");
    printf("\t1 = xenon\n");
    printf("\t2 = LXe vessel\n");
    printf("\t3 = hfe\n");
    printf("\t4 = inner cyrostat\n");
    printf("\t5 = outer cryostat\n");
    printf("\t6 = lead\n");
    scanf("%d",&ivolume);

  }

  if ( igen == 3 ) {

    printf("Enter the Z and A for the primary nucleus\n");
    scanf("%d %d",&zion, &aion);

    zmin = 0;
    zmax = 0;
    amin = 0;
    amax = 0;

    printf("Would you like all other ions to be stable? (y,n)\n");
    c = getchar();
    while ( c != 'y' && c != 'n' ) {
      c = getchar();
    }
    if ( c == 'y' ) {
      zmin = zion;
      zmax = zion;
      amin = aion;
      amax = aion;
    }
    else {
      printf("Would you like to specify which nuclei should be unstable? (y,n)\n");
      c = getchar();
      while ( c != 'y' && c != 'n' ) {
        c = getchar();
      }
      if ( c == 'y' ) {
        printf("Enter the Zmin, Zmax, Amin, and Amax for the decaying ions\n");
        scanf("%d %d %d %d",&zmin, &zmax, &amin, &amax);
	if ( zion < zmin || zion > zmax ||
	     aion < amin || aion > amax ) {
	  printf("primary nucleus will be stable with these limits\n");
	  return;
	}
      }
      else {
        printf("Will use actual stability for all ions.\n");
      }
    }

  } // primary is a radioactive ion

  if ( igen == 5 ) {
    zmin = 0;
    zmax = 0;
    amin = 0;
    amax = 0;
    zion = 86;
    aion = 222;
  }

  if ( igen == 6 ) {
    zmin = 0;
    zmax = 0;
    amin = 0;
    amax = 0;
    zion = 92;
    aion = 238;
  }

  if ( igen == 7 ) {
    zmin = 0;
    zmax = 0;
    amin = 0;
    amax = 0;
    zion = 86;
    aion = 220;
  }

  if ( igen == 8 ) {
    zmin = 0;
    zmax = 0;
    amin = 0;
    amax = 0;
    zion = 90;
    aion = 232;
  }

  if ( igen == 9 ) {
    printf("Set Xenon Rayleigh Length (in cm, use floating point number with decimal)\n");
    scanf("%f",&xenon_rayleigh);

    printf("Set Xenon Absorption Length (cm)\n");
    scanf("%f",&xenon_absl);

    printf("Set Xenon Scintillation Yield (photons/MeV)\n");
    scanf("%f",&xenon_scint_yield);

    printf("Set Alpha Energy (keV)\n");
    scanf("%f",&alpha_energy);

    printf("Set Fano Factor\n");
    scanf("%f",&fano_factor);
  }

  make_mac_files();

}

void make_mac_files()

{

  FILE *fptr;

  sprintf(ifilename,"%s.mac",filename);
      
  if (  ( fptr = fopen( filename, "w" ) ) == NULL ) {
    printf("can't open output file: %s\n", filename );
    return;
  }

  fprintf(fptr,"/digitizer/wireNoise %f\n", wirenoise);
  
  fprintf(fptr,"/digitizer/APDNoise %f\n", APDnoise);
  
  fprintf(fptr,"/digitizer/LXeEnergyRes %f\n", energy_res);
  
  if ( lxe_only == 1) {
    fprintf(fptr,"/event/LXeEventsOnly true\n");
  }
  else {
    fprintf(fptr,"/event/LXeEventsOnly false\n");
  }
  
  if ( calc_signals == 1) {
    fprintf(fptr,"/event/digitizeWires true\n");
    fprintf(fptr,"/event/digitizeAPDs true\n");
    if ( digitime >= 1.0 ) {
      fprintf(fptr,"/event/totalDigitizationTime %f microsecond\n",
	      digitime);
    }
  }
  else {
    fprintf(fptr,"/event/digitizeWires false\n");
    fprintf(fptr,"/event/digitizeAPDs false\n");
  }
  
  if ( igen == 1 ) {
    fprintf(fptr,"/generator/setGenerator bb0n\n");
  }
  else if ( igen == 2 ) {
    fprintf(fptr,"/generator/setGenerator bb2n\n");
  }
  else if ( igen > 2 ) {
    
    fprintf(fptr,"/gps/pos/type Volume\n");
    fprintf(fptr,"/gps/pos/shape Cylinder\n");
    fprintf(fptr,"/gps/pos/halfz ");
    if ( ivolume == 1 ) {
      fprintf(fptr,"21.6 cm\n");
    }
    else if ( ivolume == 2) {
      fprintf(fptr,"26.6 cm\n");
    }
    else if ( ivolume == 3) {
      fprintf(fptr,"72.5 cm\n");
    }
    else if ( ivolume == 4) {
      fprintf(fptr,"74.5 cm\n");
    }
    else if ( ivolume == 5) {
      fprintf(fptr,"79.5 cm\n");
    }
    else if ( ivolume == 6) {
      fprintf(fptr,"105.0 cm\n");
    }
    fprintf(fptr,"/gps/pos/radius ");
    if ( ivolume == 1 ) {
      fprintf(fptr,"21.6 cm\n");
    }
    else if ( ivolume == 2) {
      fprintf(fptr,"26.6 cm\n");
    }
    else if ( ivolume == 3) {
      fprintf(fptr,"75.0 cm\n");
    }
    else if ( ivolume == 4) {
      fprintf(fptr,"78.0 cm\n");
    }
    else if ( ivolume == 5) {
      fprintf(fptr,"84.0 cm\n");
    }
    else if ( ivolume == 6) {
      fprintf(fptr,"110.0 cm\n");
    }
    fprintf(fptr,"/gps/pos/centre 0.0 0.0 0.0 cm\n");
    fprintf(fptr,"/gps/pos/confine ");
    if ( ivolume == 1 ) {
      fprintf(fptr,"ActiveLXe\n");
    }
    else if ( ivolume == 2) {
      fprintf(fptr,"LXeVessel\n");
    }
    else if ( ivolume == 3) {
      fprintf(fptr,"HFE\n");
    }
    else if ( ivolume == 4) {
      fprintf(fptr,"InnerCryo\n");
    }
    else if ( ivolume == 5) {
      fprintf(fptr,"OuterCryo\n");
    }
    else if ( ivolume == 6) {
      fprintf(fptr,"LeadShield\n");
    }
    
    if ( igen == 9) {
      fprintf(fptr,"/gps/energy %f keV\n",alpha_energy);
      fprintf(fptr,"/gps/particle alpha\n");
      fprintf(fptr,"/grdm/analogueMC 1\n");
      fprintf(fptr,"/detector/setXenonABSL %f cm\n",xenon_absl);
      fprintf(fptr,"/detector/setXenonRayleigh %f cm\n",xenon_rayleigh);
      fprintf(fptr,"/detector/setXenonScintFano %f\n",fano_factor);
      fprintf(fptr,"/detector/setXenonScintYield %f\n",xenon_scint_yield);
    }
    else {
      fprintf(fptr,"/gps/energy 0 keV\n");
      fprintf(fptr,"/gps/particle ion\n");
      fprintf(fptr,"/gps/ion %d %d 0 0\n",zion,aion);
      fprintf(fptr,"/grdm/analogueMC 1\n");
      if ( amin != 0 ) {
	fprintf(fptr,"/grdm/nucleusLimits %d %d %d %d\n",amin, amax, zmin, zmax);
      }
    }
  }
  
  fclose(fptr);

}




