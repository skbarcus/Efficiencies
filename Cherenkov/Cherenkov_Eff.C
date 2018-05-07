void Cherenkov_Eff()
{  
  Int_t runlist[5] = {3892, 3893, 3894, 4074, 4075}; 
  Double_t shmin=0.65;
  Double_t thmin=-0.07, thmax=0.07;
  Double_t phmin=-0.05, phmax=0.05;
  Double_t dpmin=-0.05, dpmax=0.05;
      
  for(Int_t i=0;i<5;i++)
    {
      Int_t run=runlist[i];
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
      T->SetBranchStatus("L.cer.trx",1);
      T->SetBranchStatus("L.cer.try",1);
      T->SetBranchStatus("L.tr.tg_th",1);
      T->SetBranchStatus("L.tr.tg_ph",1);
      T->SetBranchStatus("L.tr.tg_dp",1);
      T->SetBranchStatus("L.tr.p",1);
      
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

      //Define some cuts.
      TCut ct_ep = Form("(L.prl1.e+L.prl2.e)/(1000*L.tr.p)>%f",shmin);
      TCut ct_1tr = Form("L.tr.n==1");
      TCut ct_th =Form("L.tr.tg_th>%f&&L.tr.tg_th<%f",thmin,thmax);
      TCut ct_ph =Form("L.tr.tg_ph>%f&&L.tr.tg_ph<%f",phmin,phmax);
      TCut ct_dp =Form("L.tr.tg_dp>%f&&L.tr.tg_dp<%f",dpmin,dpmax);
      TCut ct_T3 =Form("(DBB.evtypebits&1<<3)==1<<3");
      TCut ct_T7 =Form("(DBB.evtypebits&1<<7)==1<<7");

      TCanvas* c1=new TCanvas("c1");
      c1->SetGrid();
      //T->Draw("L.cer.trx:L.cer.try>>h1(200,-1.,1.,200,-1.,1.)",ct_T3&&ct_1tr&&ct_th&&ct_ph&&ct_dp&&ct_ep,"colz");
      T->Draw("L.cer.trx:L.cer.try>>h1(200,-1.,1.,200,-1.,1.)",ct_1tr&&ct_T7&&ct_ep&&ct_th&&ct_ph&&ct_dp,"colz");

      TCanvas* c2=new TCanvas("c2");
      c2->SetGrid();
      T->Draw("L.cer.trx:L.cer.try>>h2(200,-1.,1.,200,-1.,1.)",ct_1tr&&ct_T3&&ct_T7&&ct_ep&&ct_th&&ct_ph&&ct_dp,"colz");
}