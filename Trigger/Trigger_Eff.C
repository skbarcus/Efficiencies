void Trigger_Eff()
{  
  Int_t runlist[6] = {3892, 3893, 3894, 4073, 4074, 4075}; 
  Double_t Trig_eff[6] = {};
  Int_t ps_T4[6] = {3,3,3,2,3,3};
  
  for(Int_t i=0;i<6;i++)
    {
      /*    
	    m = (TH1F*)gROOT->FindObject("h1");
	    
  if(m) 
  {
  m->Delete();
      cout<<"!!!!!!"<<endl;
    }
  n = (TH1F*)gROOT->FindObject("h2");
  if(n) 
    {
      n->Delete();
      cout<<"!!!!!!"<<endl;
    }
  
  p = (TCanvas*)gROOT->FindObject("c1");
  cout<<p<<endl;
  if(p) 
    {
      p->Delete();
      cout<<"!!!!!!"<<endl;
    }
*/
      if(i>0)
	{
	  //c1->Close();
	  //c2->Close();
	  //c1->Delete();
	  //c2->Delete();
	}
      run = runlist[i];
      
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
      
      Double_t x_bj=0.,L_tr_tg_y[21],L_cer=0.,L_prl1_e=0.,L_prl2_e=0.,L_tr_p[21],L_tr_n=0.,evtypebits;
      
      T->SetBranchAddress("EKL.x_bj",&x_bj);
      T->SetBranchAddress("L.tr.tg_y",L_tr_tg_y);
      T->SetBranchAddress("L.tr.n",&L_tr_n);
      T->SetBranchAddress("L.cer.asum_c",&L_cer);
      T->SetBranchAddress("L.prl1.e",&L_prl1_e);
      T->SetBranchAddress("L.prl2.e",&L_prl2_e);
      //T->SetBranchAddress("L.tr.p",L_tr_p);
      T->SetBranchAddress("DBB.evtypebits",&evtypebits);
      
      Int_t nevts = T->GetEntries();
      cout<<"nevts = "<<nevts<<endl;
      //TH1D *h1 = new TH1D("h1","Xbj" , 1000, 0., 4.);
      //TH1D *h2 = new TH1D("h2","Xbj" , 1000, 0., 4.);
      TCanvas* c1=new TCanvas("c1");
      
      c1->SetGrid();
      c1->SetLogy();
      //T->Draw("EKL.x_bj>>h1(1000,0.,10.)","(DBB.evtypebits&1<<3)==1<<3 && L.tr.n==1");
      T->Draw(Form("EKL.x_bj>>h%d(1000,0.,10.)",run),"(DBB.evtypebits&1<<3)==1<<3 && L.tr.n==1");
      
      TCanvas* c2=new TCanvas("c2");
      c2->SetGrid();
      c2->SetLogy();
      T->Draw(Form("EKL.x_bj>>h%dcut(1000,0.,10.)",run),"(DBB.evtypebits&1<<4)==1<<4 && L.tr.n==1");
      
      Double_t psT3 = 1.;
      Double_t psT4 = ps_T4[i];
      
      //Define the name of each histo for each run. There should be hrun# and hrun#cut.
      char name[20];
      sprintf(name, "%s%d","h",run);
      TH1F *h = (TH1F*)gDirectory->Get(name);
      //Double_t nT3 = h3892->GetEntries();
      Double_t nT3 = h->GetEntries();
      //Double_t nT3 = Form("%s%d","h",run)->GetEntries();
      //Double_t nT3 = Form("h%d",run)->GetEntries();
      //Double_t nT3 = hForm("",run)->GetEntries();
      
      char namecut[20];
      sprintf(namecut, "%s%d%s","h",run,"cut");
      TH1F *hcut = (TH1F*)gDirectory->Get(namecut);
      Double_t nT4 = hcut->GetEntries();
      Double_t effT3 = 0.;
      
      effT3 = (psT3*nT3)/(psT3*nT3+psT4*nT4);
      Trig_eff[i] = effT3;
      cout<<"Total T3 = "<<psT3*nT3<<"   Total T4 = "<<psT4*nT4<<"   Total T3+T4 = "<<psT3*nT3+psT4*nT4<<endl;
      cout<<"Trigger efficiency for T3 = "<<effT3<<endl;
      
      //h1->Delete();
      //h2->Delete();
      //c1->Clear();
      //c2->Clear();
      //c1->Close();
      //c2->Close();
      //c1->Delete();
      //c2->Delete();
    }
  TH1D *h1 = new TH1D("h1","T3 Efficiencies by Run Number" , 6, 0., 6.);
  for(Int_t i=0;i<6;i++)
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
  //h1->GetXaxis()->CenterTitle();
  h1->GetYaxis()->SetTitle("Efficiency of T3");
  // h1->GetYaxis()->CenterTitle();
  //h1->GetYaxis()->SetTitleOffset(1.4);
  gStyle->SetTitleFontSize(0.08);
  h1->GetYaxis()->CenterTitle(true);
  h1->GetYaxis()->SetLabelSize(0.035);
  h1->GetYaxis()->SetTitleSize(0.06);
  h1->GetYaxis()->SetTitleOffset(0.75);
  h1->GetXaxis()->CenterTitle(true);
  h1->GetXaxis()->SetLabelSize(0.05);
  h1->GetXaxis()->SetTitleSize(0.06);
  h1->GetXaxis()->SetTitleOffset(0.75);
}
