

void E_P_3He(Int_t run)
{
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
  T->SetBranchStatus("L.cer.asum_c",1);
  T->SetBranchStatus("L.prl1.e",1);
  T->SetBranchStatus("L.prl2.e",1);
  T->SetBranchStatus("L.tr.p",1);

  Double_t shmin = 0.8;
  TCut ct1 = Form("(L.prl1.e+L.prl2.e)/(1000*L.tr.p)>%f",shmin);

  T->Draw("(L.prl1.e+L.prl2.e)/(1000*L.tr.p)>>h1(1000,0,2)");
  h1->SetTitle("(L.prl1.e+L.prl2.e)/L.tr.p");
  h1->SetTitleSize(0.05,"X");
  h1->SetXTitle("(L.prl1.e+L.prl2.e)/L.tr.p");
  //pad2->cd(2)->Update();
  //ymax = pad2->cd(2)->GetUymax();
  //TLine *line3 = new TLine(shmin,0,shmin,pow(10,ymax));
  //line3->SetLineColor(2);
  //line3->SetLineWidth(2);
  //line3->Draw();

}
