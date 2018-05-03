



void Cher_Cut_Eff_3He(Int_t run)
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
  T->SetBranchStatus("L.tr.n",1);

  Double_t L_cer=0.,L_prl1_e=0.,L_prl2_e=0.,L_tr_p=0.,L_tr_n=0.;//,data;
  T->SetBranchAddress("L.cer.asum_c",&L_cer);
  T->SetBranchAddress("L.prl1.e",&L_prl1_e);
  T->SetBranchAddress("L.prl2.e",&L_prl2_e);
  T->SetBranchAddress("L.tr.p",L_tr_p);      //If add & before L_tr_p breaks code.
  T->SetBranchAddress("L.tr.n",&L_tr_n);
  //T->SetBranchAddress("Ndata.L.tr.p",&data);

  /*
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
  */
  Int_t show_sample_regions_in_opposite = 1;
  Int_t show_sample_regions = 0;
  Double_t ymax = 10000;
  Double_t GC_cut = 150.;
  Double_t nevts = T->GetEntries(); 
  Double_t nevts_ev = ev->GetEntries();

  cout<<"Number of entries in T = "<<nevts<<endl;

  //Define cuts on the Cherenkov spectrum to locate pions.
  Double_t gc_pi_lower = 0.0;           //Lower cut on GC to select pions.
  Double_t gc_pi_upper = 10.0;           //Upper cut on GC to select pions.

  //Define cuts on the Cherenkov spectrum to locate good electrons.
  Double_t gc_e_lower = 300.0;         //Lower cut on GC to select good e-.
  Double_t gc_e_upper = 350.0;         //Upper cut on GC to select good e-.

  //Define cuts on the pion rejectors to locate pions.
  Double_t pr_pi_left = 100.0;          //Left cut on pion rejectors to select pions.
  Double_t pr_pi_right = 175.0;         //Right cut on pion rejectors to select pions.
  Double_t pr_pi_lower = 100.0;         //Lower cut on pion rejectors to select pions.
  Double_t pr_pi_upper = 175.0;         //Upper cut on pion rejectors to select pions.

  //Define cuts on the pion rejectors to locate good electrons.
  Double_t pr_e_left = 0.0;        //Left cut on pion rejectors to select good e-.
  Double_t pr_e_right = 0.0;       //Right cut on pion rejectors to select good e-.
  Double_t pr_e_lower = 0.0;       //Lower cut on pion rejectors to select good e-.
  Double_t pr_e_upper = 0.0;       //Upper cut on pion rejectors to select good e-.
  
  Double_t slope_e = 1.0;
  //Pion rejector good electron cuts.
  //Bottom line.
  TF1 *line9 = new TF1("line9",Form("-%f*x+2850.",slope_e), 600.0, 1300.);
  //Top line.
  TF1 *line10 = new TF1("line10",Form("-%f*x+3150.",slope_e), 700.0, 1400.);
  //Left line.
  TF1 *line11 = new TF1("line11",Form("%f*x+1500.",slope_e), 600.0, 900.);
  //Right line.
  TF1 *line12 = new TF1("line12",Form("%f*x+500.",slope_e), 1100.0, 1400.);
  //Pion rejector diagonal pion cut (could just do horizontal).
  TF1 *line13 = new TF1("line13",Form("-%f*x+2000.",slope_e), 0.0, 3000.);

  //Define an E/P cut.
  Double_t shmin = 0.8;
  TCut ct_ep = Form("(L.prl1.e+L.prl2.e)/(1000*L.tr.p)>%f",shmin);
  TCut ct_gc_pi =  Form("%f<L.cer.asum_c&&L.cer.asum_c<%f",gc_pi_lower,gc_pi_upper);
  TCut ct_gc_e =  Form("%f<L.cer.asum_c&&L.cer.asum_c<%f",gc_e_lower,gc_e_upper);
  TCut ct_pr_pi = Form("L.prl2.e>%f&&L.prl2.e<%f&&L.prl1.e>%f&&L.prl1.e<%f",pr_pi_left,pr_pi_right,pr_pi_lower,pr_pi_upper);
  TCut ct_1tr = Form("L.tr.n==1");
  //TCut ct_pr_e = Form("(L.prl1.e+L.prl2.e)/(1000*L.tr.p)>%f&&(L.prl1.e+L.prl2.e)/(1000*L.tr.p)<%f&&(L.prl1.e+L.prl2.e)/(1000*L.tr.p)>%f&&(L.prl1.e+L.prl2.e)/(1000*L.tr.p)>%f&&(L.prl1.e+L.prl2.e)/(1000*L.tr.p)<%f",line11->Eval((L_prl1_e+L_prl2_e)/(1000*L_tr_p)),line12->Eval((L_prl1_e+L_prl2_e)/(1000*L_tr_p)),line9->Eval((L_prl1_e+L_prl2_e)/(1000*L_tr_p)),line10->Eval((L_prl1_e+L_prl2_e)/(1000*L_tr_p)));
  //L_prl1_e>line9->Eval(L_prl2_e)&&L_prl1_e<line10->Eval(L_prl2_e)&&L_prl1_e>line12->Eval(L_prl2_e)&&L_prl1_e<line11->Eval(L_prl2_e)

  //TCut ct_pr_e = Form("L.prl1.e>%f",line9->Eval(L_prl2_e));

  TCanvas* c1=new TCanvas("c1");
  c1->SetGrid();
  //TPad *pad1 = new TPad("pad1","pad1",0.0,0.0,1.0,0.96);
  //pad1->Draw();
  //pad1->Divide(1,2);
  c1->Divide(1,2);

  c1->cd(1);
  //Draw the Cherenkov spectrum.
  T->Draw("L.cer.asum_c>>h1(1500,0,1500)");
  h1->GetYaxis()->SetTitle("Counts");
  h1->GetXaxis()->SetTitle("L.cer.asum_c");
  h1->SetMaximum(ymax);
  h1->GetYaxis()->SetTitleOffset(1.3);
  gPad->SetLogy();
  
  Int_t cermin = 300;
  
  //Draw a line representing the Cherenkov cut and tie it to the slider.
  //TLine *line1 = new TLine(cermin, 0, cermin, ymax);
  //line1->SetLineColor(kRed);
  //line1->DrawLine(slidervalue,0,slidervalue,ymax);

  //Draw GC cuts.
  c1->Update();
  //pad1->cd(1)->Update();

  TLine *lineGC_cut = new TLine(GC_cut, 0, GC_cut, h1->GetBinContent(GC_cut));
  lineGC_cut->SetLineColor(kGreen);
  lineGC_cut->Draw("same");
  
  TLine *line1 = new TLine(gc_pi_lower, 0, gc_pi_lower, h1->GetBinContent(gc_pi_lower));
  line1->SetLineColor(kRed);
  line1->Draw("same");

  TLine *line2 = new TLine(gc_pi_upper, 0, gc_pi_upper, h1->GetBinContent(gc_pi_upper));
  line2->SetLineColor(kRed);
  line2->Draw("same");
  //cout<<h1->GetMinimum()<<endl;    //Gives min bin value for data in histo.
  //cout<<h1->GetMaximum()<<endl;    //Gives max bin value for data in histo (limited by histo max).
  //cout<<h1->GetXaxis()->GetXmin()<<endl;
  //cout<<h1->GetXaxis()->GetXmax()<<endl;
  //h1->GetXaxis()->SetRange(300,500);
  //cout<<h1->GetMaximum()<<endl;
  
  /*
   TLine l;
   Double_t lm = gPad->GetLeftMargin();
   Double_t rm = 1.-gPad->GetRightMargin();
   Double_t tm = 1.-gPad->GetTopMargin();
   Double_t bm = gPad->GetBottomMargin();
   Double_t xndc = (rm-lm)*((100.0-gPad->GetUxmin())/(gPad->GetUxmax()-gPad->GetUxmin()))+lm;
   l.DrawLineNDC(xndc,bm,xndc,tm);
  */

  TLine *line3 = new TLine(gc_e_lower, 0, gc_e_lower, h1->GetBinContent(gc_e_lower));
  line3->SetLineColor(kRed);
  line3->Draw("same");

  TLine *line4 = new TLine(gc_e_upper, 0, gc_e_upper, h1->GetBinContent(gc_e_upper));
  line4->SetLineColor(kRed);
  line4->Draw("same");

  c1->cd(2);
  
  //Draw the shower and preshower with the Cherenkov cut applied.
  T->Draw("L.prl1.e:L.prl2.e>>h2(1000,80.,3000,1000,80.,3200)","","colz");
  h2->GetYaxis()->SetTitle("L.prl1.e");
  h2->GetXaxis()->SetTitle("L.prl2.e");
  h2->GetYaxis()->SetTitleOffset(1.55);

  //Draw pion rejector cuts.
  c1->Update();
  //Pion rejector pion cuts.
  //Bottom line.
  TLine *line5 = new TLine(pr_pi_left, pr_pi_lower, pr_pi_right, pr_pi_lower);
  line5->SetLineColor(kRed);
  line5->Draw("same");
  //Top line.
  TLine *line6 = new TLine(pr_pi_left, pr_pi_upper, pr_pi_right, pr_pi_upper);
  line6->SetLineColor(kRed);
  line6->Draw("same");
  //Left line.
  TLine *line7 = new TLine(pr_pi_left, pr_pi_lower, pr_pi_left, pr_pi_upper);
  line7->SetLineColor(kRed);
  line7->Draw("same");
  //Right line.
  TLine *line8 = new TLine(pr_pi_right, pr_pi_lower, pr_pi_right, pr_pi_upper);
  line8->SetLineColor(kRed);
  line8->Draw("same");

  line9->Draw("same");
  line10->Draw("same");
  line11->Draw("same");
  line12->Draw("same");
  line13->Draw("same");
  line13->SetLineColor(kGreen);

  if(show_sample_regions_in_opposite==1)
    {
  //Draw calo corresponding to e- and pion samples in the GC.
  TCanvas* c6=new TCanvas("c6");
  c6->SetGrid();
  c6->Divide(1,2);
  
  c6->cd(1);
  TH2D *h11 = new TH2D("h11",Form("Good e- Region from GC in Pion Rejectors for Run %i Cut: %.1f<GC<%.1f",run,gc_e_lower,gc_e_upper) , 1000, 80., 3000., 1000, 80., 3200.);
  Double_t e_tot_ct_pr,e_lost_ct_pr = 0.;

  for(Int_t i=0;i<nevts;i++) 
    {
      T->GetEntry(i);
      if(L_cer>gc_e_lower && L_cer<gc_e_upper)
	{
	  h11->Fill(L_prl2_e,L_prl1_e);
	  e_tot_ct_pr++;
	  if(L_prl1_e<line13->Eval(L_prl2_e))
	    {
	      e_lost_ct_pr++;
	    }
	}
    }
  h11->Draw("colz");
  line13->Draw("same");
  cout<<"Good e- Region from GC has "<<e_tot_ct_pr<<" entries. Diagonal cut on PR causes "<<e_lost_ct_pr<<" likely good e- to be lost, or a loss of "<<(e_lost_ct_pr/e_tot_ct_pr)*100.<<"% of good e-."<<endl;
  
  c6->cd(2);
  TH2D *h12 = new TH2D("h12",Form("Pion Region from GC in Pion Rejectors for Run %i Cut: %.1f<GC<%.1f",run,gc_pi_lower,gc_pi_upper) , 1000, 80., 3000., 1000, 80., 3200.);
  Double_t pi_tot_ct_pr,pi_lost_ct_pr = 0.;

  for(Int_t i=0;i<nevts;i++) 
    {
      T->GetEntry(i);
      if(L_cer>gc_pi_lower && L_cer<gc_pi_upper)
	{
	  h12->Fill(L_prl2_e,L_prl1_e);
	  pi_tot_ct_pr++;
	  if(L_prl1_e<line13->Eval(L_prl2_e))
	    {
	      pi_lost_ct_pr++;
	    }
	}
    }
  h12->Draw("colz");
  line13->Draw("same");
  cout<<"Pion Region from GC has "<<pi_tot_ct_pr<<" entries. Diagonal cut on PR causes "<<pi_lost_ct_pr<<" likely pions to be lost, or a loss of "<<(pi_lost_ct_pr/pi_tot_ct_pr)*100.<<"% of pions."<<endl;
  
  //Draw GC corresponding to e- and pion samples in the calo.
  TCanvas* c7=new TCanvas("c7");
  c7->SetGrid();

  TH1D *h13 = new TH1D("h13",Form("Good e- Region from PR in GC for Run %i with Region Cuts",run) , 1500, 0., 1500.);
  Double_t e_tot_ct_gc = 0.0, e_lost_ct_gc = 0.0;

  for(Int_t i=0;i<nevts;i++) 
    {
      T->GetEntry(i);
      if(L_prl1_e>line9->Eval(L_prl2_e)&&L_prl1_e<line10->Eval(L_prl2_e)&&L_prl1_e>line12->Eval(L_prl2_e)&&L_prl1_e<line11->Eval(L_prl2_e))
	{
	  h13->Fill(L_cer);
	  e_tot_ct_gc++;
	  if(L_cer<GC_cut)
	    {
	      e_lost_ct_gc++;
	    }
	}
    }
  h13->Draw("");
  h13->SetLineColor(kBlack);
  cout<<"Good e- Region from PR has "<<e_tot_ct_gc<<" entries. GC_cut = "<<GC_cut<<" causes "<<e_lost_ct_gc<<" likely good e- to be lost, or a loss of "<<(e_lost_ct_gc/e_tot_ct_gc)*100.<<"% of good e-."<<endl;
  
  TH1D *h14 = new TH1D("h14",Form("Pion Region from PR in GC for Run %i with Region Cuts",run) , 1500, 0., 1500.);
  Double_t pi_tot_ct_gc = 0.0, pi_lost_ct_gc = 0.0;

  for(Int_t i=0;i<nevts;i++) 
    {
      T->GetEntry(i);
      if(L_prl2_e>pr_pi_left && L_prl2_e<pr_pi_right && L_prl1_e>pr_pi_lower && L_prl1_e<pr_pi_upper)
	{
	  h14->Fill(L_cer);
	  pi_tot_ct_gc++;
	  if(L_cer<GC_cut)
	    {
	      pi_lost_ct_gc++;
	    }
	}
    }
  h14->Draw("same");
  h14->SetLineColor(kRed);
  lineGC_cut->Draw("same");
  cout<<"Pion Region from PR has "<<pi_tot_ct_gc<<" entries. GC_cut = "<<GC_cut<<" causes "<<pi_lost_ct_gc<<" likely pions to be lost, or a loss of "<<(pi_lost_ct_gc/pi_tot_ct_gc)*100.<<"% of pions."<<endl;
    }// End show_sample_regions_in_opposite

  //Draw sample e- and pion regions with the other detector's PID cut. e.g. draw PR good e- region with a cut on the GC etc.
  if(show_sample_regions==1)
    {
      TCanvas* c2=new TCanvas("c2");
      c2->SetGrid();
      c2->Divide(1,2);
      
      c2->cd(1);
      TH2D *h3 = new TH2D("h3",Form("Good e- Region L.prl1.e vs. L.prl2.e for Run %i No Cuts",run) , 3000, 0., 3000., 3200, 0., 3200.);
      
      for(Int_t i=0;i<nevts;i++)        //Breaks above i<142? This was due to L.tr.p having numerous entries (more than 2 maybe 3). When printed L.tr.p says Nnata.L.tr.p and seems to be an array so can't have & in BranchAddress.
	{
	  T->GetEntry(i);
	  //T->Show(i);
	  if(L_prl1_e>line9->Eval(L_prl2_e)&&L_prl1_e<line10->Eval(L_prl2_e)&&L_prl1_e>line12->Eval(L_prl2_e)&&L_prl1_e<line11->Eval(L_prl2_e))
	    {
	      h3->Fill(L_prl2_e,L_prl1_e);
	    }
	}
      h3->Draw("colz");
      
      //Draw good electron region of pr with gc cuts applied giving number of good e- lost to the GC cuts.
      c2->cd(2);
      TH2D *h4 = new TH2D("h4",Form("Good e- Region L.prl1.e vs. L.prl2.e for Run %i with GC Cut=%.1f",run,GC_cut) , 3000, 0., 3000., 3200, 0., 3200.);
      
      for(Int_t i=0;i<nevts;i++)        //Breaks above i<142? This was due to L.tr.p having numerous entries (more than 2 maybe 3). When printed L.tr.p says Nnata.L.tr.p and seems to be an array so can't have & in BranchAddress.
	{
	  T->GetEntry(i);
	  //T->Show(i);
	  if(L_prl1_e>line9->Eval(L_prl2_e)&&L_prl1_e<line10->Eval(L_prl2_e)&&L_prl1_e>line12->Eval(L_prl2_e)&&L_prl1_e<line11->Eval(L_prl2_e) && L_cer>GC_cut)
	    {
	      h4->Fill(L_prl2_e,L_prl1_e);
	    }
	}
      h4->Draw("colz");
      
      cout<<"Electrons lost in good e- region after GC cut applied = "<<h4->GetEntries()<<"/"<<h3->GetEntries()<<" = "<<h4->GetEntries()/h3->GetEntries()<<" = "<<(1-h4->GetEntries()/h3->GetEntries())*100.<<"% of electrons."<<endl;
      
      //Draw good pion region in pr with GC cuts giving the number of pions removed from the sample by GC cuts.
      TCanvas* c3=new TCanvas("c3");
      c3->SetGrid();
      c3->Divide(1,2);
      
      c3->cd(1);
      TH2D *h5 = new TH2D("h5",Form("Pion Region L.prl1.e vs. L.prl2.e for Run %i No Cuts",run) , 3000, 0., 3000., 3200, 0., 3200.);
      
      for(Int_t i=0;i<nevts;i++)        //Breaks above i<142? This was due to L.tr.p having numerous entries (more than 2 maybe 3). When printed L.tr.p says Nnata.L.tr.p and seems to be an array so can't have & in BranchAddress.
	{
	  T->GetEntry(i);
	  //T->Show(i);
	  if(L_prl2_e>pr_pi_left && L_prl2_e<pr_pi_right && L_prl1_e>pr_pi_lower && L_prl1_e<pr_pi_upper)
	    {
	      h5->Fill(L_prl2_e,L_prl1_e);
	    }
	}
      h5->Draw("colz");
      
      //Draw good pion region of pr with gc cuts applied.
      c3->cd(2);
      TH2D *h6 = new TH2D("h6",Form("Pion Region L.prl1.e vs. L.prl2.e for Run %i with GC Cut=%.1f",run,GC_cut) , 3000, 0., 3000., 3200, 0., 3200.);
      
      for(Int_t i=0;i<nevts;i++)        //Breaks above i<142? This was due to L.tr.p having numerous entries (more than 2 maybe 3). When printed L.tr.p says Nnata.L.tr.p and seems to be an array so can't have & in BranchAddress.
	{
	  T->GetEntry(i);
	  //T->Show(i);
	  if(L_prl2_e>pr_pi_left && L_prl2_e<pr_pi_right && L_prl1_e>pr_pi_lower && L_prl1_e<pr_pi_upper && L_cer>GC_cut)
	    {
	      h6->Fill(L_prl2_e,L_prl1_e);
	    }
	}
      h6->Draw("colz");
      
      cout<<"Pions remaining after GC cut applied = "<<h6->GetEntries()<<"/"<<h5->GetEntries()<<" = "<<h6->GetEntries()/h5->GetEntries()<<" = "<<(h6->GetEntries()/h5->GetEntries())*100.<<"% of pions survive GC cut."<<endl;
      
      
      //Draw good e- region in GC  with pr cuts giving the number of good e-s removed from the sample by pr cuts.
      TCanvas* c3=new TCanvas("c4");
      c4->SetGrid();
      c4->Divide(1,2);
      
      c4->cd(1);
      TH1D *h7 = new TH1D("h7",Form("Good e- Region of GC for Run %i No Cuts",run) , 1500, 0., 1500.);
      
      for(Int_t i=0;i<nevts;i++)        //Breaks above i<142? This was due to L.tr.p having numerous entries (more than 2 maybe 3). When printed L.tr.p says Nnata.L.tr.p and seems to be an array so can't have & in BranchAddress.
	{
	  T->GetEntry(i);
	  //T->Show(i);
	  if(L_cer>gc_e_lower && L_cer<gc_e_upper)
	    {
	      h7->Fill(L_cer);
	    }
	}
      h7->Draw("");
      
      //Draw good e- region of calo with pr cuts applied.
      c4->cd(2);
      TH1D *h8 = new TH1D("h8",Form("Good e- Region of GC for Run %i with PR Cut",run) , 1500, 0., 1500.);
      
      for(Int_t i=0;i<nevts;i++)        //Breaks above i<142? This was due to L.tr.p having numerous entries (more than 2 maybe 3). When printed L.tr.p says Nnata.L.tr.p and seems to be an array so can't have & in BranchAddress.
	{
	  T->GetEntry(i);
	  //T->Show(i);
	  if(L_cer>gc_e_lower && L_cer<gc_e_upper && L_prl1_e>line13->Eval(L_prl2_e))
	    {
	      h8->Fill(L_cer);
	    }
	}
      h8->Draw("");
      cout<<"Good electrons lost in GC after PR cut applied = "<<h8->GetEntries()<<"/"<<h7->GetEntries()<<" = "<<h8->GetEntries()/h7->GetEntries()<<" = "<<(h8->GetEntries()/h7->GetEntries())*100.<<"% of electrons survive PR cut."<<endl;
      
      
      //Draw good pion region in GC  with pr cuts giving the number of pions removed from the sample by pr cuts.
      TCanvas* c3=new TCanvas("c5");
      c5->SetGrid();
      c5->Divide(1,2);
      
      c5->cd(1);
      TH1D *h9 = new TH1D("h9",Form("Pion Region of GC for Run %i No Cuts",run) , 1500, 0., 1500.);
      
      for(Int_t i=0;i<nevts;i++)        //Breaks above i<142? This was due to L.tr.p having numerous entries (more than 2 maybe 3). When printed L.tr.p says Nnata.L.tr.p and seems to be an array so can't have & in BranchAddress.
	{
	  T->GetEntry(i);
	  //T->Show(i);
	  if(L_cer>gc_pi_lower && L_cer<gc_pi_upper)
	    {
	      h9->Fill(L_cer);
	    }
	}
      h9->Draw("");
      
      //Draw pion region of calo with pr cuts applied.
      c5->cd(2);
      TH1D *h10 = new TH1D("h10",Form("Pion Region of GC for Run %i with PR Cut",run) , 1500, 0., 1500.);
      
      for(Int_t i=0;i<nevts;i++)        //Breaks above i<142? This was due to L.tr.p having numerous entries (more than 2 maybe 3). When printed L.tr.p says Nnata.L.tr.p and seems to be an array so can't have & in BranchAddress.
	{
	  T->GetEntry(i);
	  //T->Show(i);
	  if(L_cer>gc_pi_lower && L_cer<gc_pi_upper && L_prl1_e>line13->Eval(L_prl2_e))
	    {
	      h10->Fill(L_cer);
	    }
	}
      h10->Draw("");
      cout<<"Pions remaining after PR cut applied = "<<h10->GetEntries()<<"/"<<h9->GetEntries()<<" = "<<h10->GetEntries()/h9->GetEntries()<<" = "<<(h10->GetEntries()/h9->GetEntries())*100.<<"% of pions survive PR cut."<<endl;
      
    }//End if show_sample_regions.
  
}
