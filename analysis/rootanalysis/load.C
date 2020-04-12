
TFile *f;
TChain *tree = NULL;

Int_t load( char * filename, bool print )
{

  // This function opens a root file, 
  // looks for a root tree name "tree", 
  // and associates that tree with a TChain called tree.

  // See if the file is empty

  TFile *testf = new TFile(filename);
  if ( testf->IsZombie() == true ) return 0 ; 
  if ( testf->GetNkeys() == 0 ) return 0;
  TTree *testtr = (TTree*)testf->Get("tree");
  Int_t nevents = (Int_t)testtr->GetEntriesFast();
  if ( print == true ) {
    cout << "Number of events in file = " << nevents << endl;
  }
  testf->Close();
  if (  nevents == 0 ) return 1;

  // Add file to the chain

  if ( tree == NULL )  {
    tree = new TChain("tree");
    tree->Add(filename);
    if ( print == true ) {
      cout << "ntuple pointer is tree" << endl;
    }
  }
  else {
    tree->Add(filename);
  }

  return 1;

}

void load( char * filename )
{
  Int_t load( char * filename, bool print );
  load(filename,true);
}

void loadall( char * filename )
{

  // Load multiple root files into a TChain.
  // Example: if bb0n_0.root, bb0n_1.root, and 
  // bb0n_2.root exist, loadall("bb0n");
  // will load them all.

  Int_t load( char * filename, bool print );

  // See how many files there are

  Int_t fail_count = 0;
  Int_t last_successful = -1;
  const Int_t maxlen = 2000;
  char existstring[maxlen];
  int i = 0;
  Int_t fnlen; 

  fnlen = strlen(filename);
   if (fnlen > maxlen){
     cout << "Maximum filename length of " << maxlen << " exceeded. Exiting." << endl;
    return; 
  }
  while ( fail_count < 10 ) {
    sprintf(existstring,"test -s %s_%d.root", filename,i);
    Int_t exist = gSystem->Exec(existstring);
    if ( exist != 0 ) {
      fail_count++;
    }
    else {
      fail_count = 0;
      last_successful = i;
    }
    i++;
  }

  // see if we have a single file instead of a set of files

  if ( last_successful == - 1 ) {
    load(filename, true);
    return;
  }

  printf("maximum file index = %d\n", last_successful);

  Int_t nfilesloaded = 0;
  char loadstring[maxlen];
  for ( i = 0; i <= last_successful; i++ ) {
     sprintf(loadstring,"%s_%d.root",filename,i);
     Int_t found_it = load(loadstring, false);
     nfilesloaded += found_it;  
  }

  cout << "number of loaded files = " << nfilesloaded << endl;

  if ( tree != NULL ) {
    cout << "Total number of events = " << 
      (Int_t)tree->GetEntries() << endl;
  }
  else {
    cout << "no events loaded" << endl;
  }

}

