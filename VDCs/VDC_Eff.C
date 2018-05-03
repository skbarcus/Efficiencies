void VDC_Eff()
{  
  //Define a new stopwatch.
  TStopwatch *st=new TStopwatch();
  st->Start(kTRUE);

  Int_t runlist[6] = {3892, 3893, 3894, 4073, 4074, 4075}; 
  Double_t VDC_eff[6] = {};
  
  for(Int_t i=0;i<6;i++)
    {
      Int_t run = runlist[i];
      
      //============  Reading the Rootfile =======================//
      
      const TString rootfilePath = "/home/skbarcus/Tritium/Analysis/He3/Rootfiles/";
      std::ostringstream str;
      str << rootfilePath<<"e08014_"<<run;
      TString basename = str.str().c_str();
      TString rootfile = basename + ".root";
      TChain* T;
      TChain*ev;
      
      if(run < 20000)
	{
	  T = new TChain("T");
	  ev = new TChain("RIGHT");
	}
      if(run > 20000)
	{
	  T = new TChain("T");
	  ev = new TChain("EVRIGHT");
	}
      
      Long_t split=0;
      
      char* file = 0;
      
      //====adding splits rootfiles =======================//
      
      Long_t u=0;
      while ( !gSystem->AccessPathName(rootfile.Data()) ) 
	{
	  T->Add(rootfile.Data());
	  ev->Add(rootfile.Data());
	  cout << "ROOT file " << rootfile << " added to TChain." << endl;
	  u++;
	  rootfile = basename + "_" + u + ".root";
	}
      
      if(!T->GetEntries())
	{
	  cerr<< "Not root file was found" << endl;
	  return;
	}
      //==finish adding splits rootfiles=====================//
      
      T->SetBranchStatus("*",0);
      T->SetBranchStatus("EKL.x_bj",1);
      T->SetBranchStatus("L.tr.tg_y",1);
      T->SetBranchStatus("L.tr.n",1);
      T->SetBranchStatus("L.cer.asum_c",1);
      T->SetBranchStatus("L.prl1.e",1);
      T->SetBranchStatus("L.prl2.e",1);
      //T->SetBranchStatus("L.tr.p",1);
      T->SetBranchStatus("DBB.evtypebits",1);
      T->SetBranchStatus("L.s2.trn",1);
      
      Double_t x_bj=0.,L_tr_tg_y[21],L_cer=0.,L_prl1_e=0.,L_prl2_e=0.,L_tr_p[21],L_tr_n=0.,evtypebits,L_s2_trn=0.;
      
      T->SetBranchAddress("EKL.x_bj",&x_bj);
      T->SetBranchAddress("L.tr.tg_y",L_tr_tg_y);
      T->SetBranchAddress("L.tr.n",&L_tr_n);
      T->SetBranchAddress("L.cer.asum_c",&L_cer);
      T->SetBranchAddress("L.prl1.e",&L_prl1_e);
      T->SetBranchAddress("L.prl2.e",&L_prl2_e);
      //T->SetBranchAddress("L.tr.p",L_tr_p);
      T->SetBranchAddress("DBB.evtypebits",&evtypebits);
      T->SetBranchAddress("L.s2.trn",&L_s2_trn);
      
      Int_t nevts = T->GetEntries();
      Double_t treq0=0.,treq1=0.,treq2=0.,treq3=0.,treq4=0.,trgte5=0.;

      cout<<"nevts = "<<nevts<<endl;
      //TH1D *h1 = new TH1D("h1","Xbj" , 1000, 0., 4.);
      //TH1D *h2 = new TH1D("h2","Xbj" , 1000, 0., 4.);
      
      for(Int_t j=0;j<nevts;j++)
	{
	  T->GetEntry(j);
	  //T->Show(j);
	  //cout<<"L.s2.trn = "<<L_s2_trn<<endl;

	  if(L_tr_n==0. && L_prl1_e>(-L_prl2_e+2000))// && L_s2_trn==1)
	    //if(L_tr_n==0. && L_cer>300 && L_cer<350)
	    {
	      treq0++; 
	      //cout<<"treq0 = "<<treq0<<endl;
	    }
	  else if(L_tr_n==1. && L_prl1_e>(-L_prl2_e+2000))// && L_s2_trn==1)
	    //else if(L_tr_n==1. && L_cer>300 && L_cer<350)
	    {
	      treq1++; 
	      //cout<<"treq1 = "<<treq1<<endl;
	    }
	  else if(L_tr_n==2. && L_prl1_e>(-L_prl2_e+2000))// && L_s2_trn==1)
	    //else if(L_tr_n==2. && L_cer>300 && L_cer<350)
	    {
	      treq2++; 
	      //cout<<"treq2 = "<<treq2<<endl;
	    }
	  else if(L_tr_n==3. && L_prl1_e>(-L_prl2_e+2000))// && L_s2_trn==1)
	    //else if(L_tr_n==3. && L_cer>300 && L_cer<350)
	    {
	      treq3++; 
	      //cout<<"treq3 = "<<treq3<<endl;
	    }
	  else if(L_tr_n==4. && L_prl1_e>(-L_prl2_e+2000))// && L_s2_trn==1)
	    //else if(L_tr_n==4. && L_cer>300 && L_cer<350)
	    {
	      treq4++; 
	      //cout<<"treq4 = "<<treq4<<endl;
	    }
	  else if(L_tr_n>=5. && L_prl1_e>(-L_prl2_e+2000))// && L_s2_trn==1)
	    //else if(L_tr_n>=5. && L_cer>300 && L_cer<350)
	    {
	      trgte5++;
	      //cout<<"trgte5 = "<<trgte5<<endl;
	    }
	}

      cout<<"treq0 = "<<treq0<<"   treq1 = "<<treq1<<"   treq2 = "<<treq2<<"   treq3 = "<<treq3<<"   treq4 = "<<treq4<<"   trgte5 = "<<trgte5<<endl;

      Double_t trtot=0.;
      trtot = treq0+treq1+treq2+treq3+treq4+trgte5;
      Double_t VDC_eff_run = 0.;
      VDC_eff_run = treq1/trtot;
      VDC_eff[i] = VDC_eff_run;
    }
  for(Int_t i=0;i<6;i++)
    {
      cout<<"VDC Efficiency for run "<<runlist[i]<<" = "<<VDC_eff[i]<<endl;
    }
  /*
    TH1D *h1 = new TH1D("h1","T3 Efficiencies by Run Number" , 7, 0., 7.);
    for(Int_t i=0;i<7;i++)
    {
    h1->SetBinContent(i+1,Trig_eff[i]);
    h1->GetXaxis()->SetBinLabel(i+1,Form("%i",runlist[i]));
    cout<<"T3 efficiency for run "<<runlist[i]<<" = "<<Trig_eff[i]<<endl;
    }
    
    TCanvas* c3=new TCanvas("c3");
    c3->SetGrid();
    gStyle->SetOptStat(0);
    h1->Draw("P");
    h1->SetMarkerColor(2);
    h1->SetMarkerSize(2);
    h1->SetMarkerStyle(33);
    h1->GetXaxis()->SetTitle("Run Number");
    h1->GetXaxis()->CenterTitle();
    h1->GetYaxis()->SetTitle("Efficiency of T3");
    h1->GetYaxis()->CenterTitle();
    h1->GetYaxis()->SetTitleOffset(1.4);
  */
  st->Stop();
  cout<<"CPU time = "<<st->CpuTime()<<" s = "<<st->CpuTime()/60.<<" min   Real time = "<<st->RealTime()<<" s = "<<st->RealTime()/60.<<" min"<<endl;
}
