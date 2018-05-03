
// Simple macro showing capabilities of triple slider
//Authors: Bertrand Bellenot, Ilka Antcheva
//Modified by Scott Barcus Feb 2017 to examine how cuts on the shower and preshower influence the Cherenkov spectrum.

#include "TGButton.h"
#include "TRootEmbeddedCanvas.h"
#include "TGLayout.h"
#include "TF1.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGTextEntry.h"
#include "TGTripleSlider.h"

enum ETestCommandIdentifiers {
   HId1,
   HId2,
   HId3,
   HId4,
   HId5,
   HId6,
   HCId1,
   HCId2,
   HSId1,
   HSId2
};

class TTripleSliderDemo : public TGMainFrame {

private:
   TRootEmbeddedCanvas *fCanvas;
   TGLayoutHints       *fLcan;
   TF1                 *fFitFcn;
   TH1                 *histo1;


   TGHorizontalFrame   *fHframe0, *fHframe1, *fHframe2, *fHframe3, *fHframe4;
   TGLayoutHints       *fBly, *fBfly1, *fBfly2, *fBfly3, *fBly4, *fBfly5, *fBfly6, *fBfly7;
   TGTripleHSlider     *fHslider1, *fHslider2;
   TGTextEntry         *fTeh1, *fTeh2, *fTeh3, *fTeh4, *fTeh5, *fTeh6;
   TGTextBuffer        *fTbh1, *fTbh2, *fTbh3, *fTbh4, *fTbh5, *fTbh6;
   TGCheckButton       *fCheck1, *fCheck2;

public:
   TTripleSliderDemo();
   virtual ~TTripleSliderDemo();

   void CloseWindow();
   void DoText(const char *text);
   void DoSlider();
   void HandleButtons();

   ClassDef(TTripleSliderDemo, 0)
};

//______________________________________________________________________________
TTripleSliderDemo::TTripleSliderDemo() : TGMainFrame(gClient->GetRoot(), 100, 100)
{

   char buf[32];
   SetCleanup(kDeepCleanup);

   // Create an embedded canvas and add to the main frame, centered in x and y
   // and with 30 pixel margins all around
   fCanvas = new TRootEmbeddedCanvas("Canvas", this, 1200, 600);
   TPad *pad1 = new TPad("pad1","pad1",0.0,0.0,1.0,0.96);
   pad1->Draw();
   pad1->Divide(2,1);
   pad1->cd(1);

   fLcan = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 10);
   AddFrame(fCanvas, fLcan);
   fCanvas->GetCanvas()->SetFillColor(33);
   fCanvas->GetCanvas()->SetFrameFillColor(41);
   fCanvas->GetCanvas()->SetBorderMode(0);
   fCanvas->GetCanvas()->SetGrid();
   fCanvas->GetCanvas()->SetLogy();

   fHframe0 = new TGHorizontalFrame(this, 0, 0, 0);

   fCheck1 = new TGCheckButton(fHframe0, "&Constrained", HCId1);
   fCheck2 = new TGCheckButton(fHframe0, "&Relative", HCId2);
   fCheck1->SetState(kButtonUp);
   fCheck2->SetState(kButtonUp);
   fCheck1->SetToolTipText("Pointer position constrained to slider sides");
   fCheck2->SetToolTipText("Pointer position relative to slider position");

   fHframe0->Resize(200, 50);

   fHframe1 = new TGHorizontalFrame(this, 0, 0, 0);

   //Create slider 1 (triple slider).
   fHslider1 = new TGTripleHSlider(fHframe1, 190, kDoubleScaleBoth, HSId1,
                                   kHorizontalFrame,
                                   GetDefaultFrameBackground(),
                                   kFALSE, kFALSE, kFALSE, kFALSE);
   fHslider1->Connect("PointerPositionChanged()", "TTripleSliderDemo",
                      this, "DoSlider()");
   fHslider1->Connect("PositionChanged()", "TTripleSliderDemo",
                      this, "DoSlider()");

   //Create slider 2.
   /*fHslider2 = new TGSlider(fHframe1, 190, kDoubleScaleBoth, HSId1,
                                   kHorizontalFrame,
                                   GetDefaultFrameBackground(),
                                   kFALSE, kFALSE, kFALSE, kFALSE);
   fHslider2->Connect("PointerPositionChanged()", "TTripleSliderDemo",
                      this, "DoSlider()");
   fHslider2->Connect("PositionChanged()", "TTripleSliderDemo",
   this, "DoSlider()");*/


   //Set slider range.
   fHslider1->SetRange(0.0,3000.0);

   fHframe1->Resize(200, 25);

   fHframe2 = new TGHorizontalFrame(this, 0, 0, 0);

   fTeh1 = new TGTextEntry(fHframe2, fTbh1 = new TGTextBuffer(5), HId1);
   fTeh2 = new TGTextEntry(fHframe2, fTbh2 = new TGTextBuffer(5), HId2);
   fTeh3 = new TGTextEntry(fHframe2, fTbh3 = new TGTextBuffer(5), HId3);

   fTeh1->SetToolTipText("Minimum (left) Value of Slider");
   fTeh2->SetToolTipText("Pointer Position Value");
   fTeh3->SetToolTipText("Maximum (right) Value of Slider");

   fTbh1->AddText(0, "0.0");
   fTbh2->AddText(0, "0.0");
   fTbh3->AddText(0, "0.0");

   fTeh1->Connect("TextChanged(char*)", "TTripleSliderDemo", this,
                  "DoText(char*)");
   fTeh2->Connect("TextChanged(char*)", "TTripleSliderDemo", this,
                  "DoText(char*)");
   fTeh3->Connect("TextChanged(char*)", "TTripleSliderDemo", this,
                  "DoText(char*)");

   fCheck1->Connect("Clicked()", "TTripleSliderDemo", this,
                    "HandleButtons()");
   fCheck2->Connect("Clicked()", "TTripleSliderDemo", this,
                    "HandleButtons()");

   fHframe2->Resize(100, 25);

   //--- layout for buttons: top align, equally expand horizontally
   fBly = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);

   //--- layout for the frame: place at bottom, right aligned
   fBfly1 = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 5, 5);
   fBfly2 = new TGLayoutHints(kLHintsTop | kLHintsLeft,    5, 5, 5, 5);
   fBfly3 = new TGLayoutHints(kLHintsTop | kLHintsRight,   5, 5, 5, 5);

   fHframe3 = new TGHorizontalFrame(this, 0, 0, 0);

   //Create slider 2.
   fHslider2 = new TGTripleHSlider(fHframe3, 190, kDoubleScaleBoth, HSId2,
                                   kHorizontalFrame,
                                   GetDefaultFrameBackground(),
                                   kFALSE, kFALSE, kFALSE, kFALSE);
   fHslider2->Connect("PointerPositionChanged()", "TTripleSliderDemo",
                      this, "DoSlider()");
   fHslider2->Connect("PositionChanged()", "TTripleSliderDemo", this, "DoSlider()");

   fHslider2->SetRange(-2.0,-0.0);

   fHframe3->Resize(200, 25);

   fHframe4 = new TGHorizontalFrame(this, 0, 0, 0);

   fTeh4 = new TGTextEntry(fHframe4, fTbh4 = new TGTextBuffer(5), HId4);
   fTeh5 = new TGTextEntry(fHframe4, fTbh5 = new TGTextBuffer(5), HId5);
   fTeh6 = new TGTextEntry(fHframe4, fTbh6 = new TGTextBuffer(5), HId6);

   fTeh4->SetToolTipText("Minimum (left) Value of Slider");
   fTeh5->SetToolTipText("Pointer Position Value");
   fTeh6->SetToolTipText("Maximum (right) Value of Slider");

   fTbh4->AddText(0, "0.0");
   fTbh5->AddText(0, "0.0");
   fTbh6->AddText(0, "0.0");

   fTeh4->Connect("TextChanged(char*)", "TTripleSliderDemo", this,
                  "DoText(char*)");
   fTeh5->Connect("TextChanged(char*)", "TTripleSliderDemo", this,
                  "DoText(char*)");
   fTeh6->Connect("TextChanged(char*)", "TTripleSliderDemo", this,
                  "DoText(char*)");

   /*fCheck3->Connect("Clicked()", "TTripleSliderDemo", this,
                    "HandleButtons()");
   fCheck4->Connect("Clicked()", "TTripleSliderDemo", this,
   "HandleButtons()");*/
  
   fHframe4->Resize(100, 25);
   
   //--- layout for buttons: top align, equally expand horizontally
   fBly4 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);

   //--- layout for the frame: place at bottom, right aligned
   fBfly5 = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 5, 5);
   fBfly6 = new TGLayoutHints(kLHintsTop | kLHintsLeft,    5, 5, 5, 5);
   fBfly7 = new TGLayoutHints(kLHintsTop | kLHintsRight,   5, 5, 5, 5);

   fHframe0->AddFrame(fCheck1, fBfly2);
   fHframe0->AddFrame(fCheck2, fBfly2);
   fHframe1->AddFrame(fHslider1, fBly);
   fHframe2->AddFrame(fTeh1, fBfly2);
   fHframe2->AddFrame(fTeh2, fBfly1);
   fHframe2->AddFrame(fTeh3, fBfly3);

   fHframe3->AddFrame(fHslider2, fBly4);
   fHframe4->AddFrame(fTeh4, fBfly6);
   fHframe4->AddFrame(fTeh5, fBfly5);
   fHframe4->AddFrame(fTeh6, fBfly7);

   AddFrame(fHframe0, fBly);
   AddFrame(fHframe1, fBly);
   AddFrame(fHframe2, fBly);

   AddFrame(fHframe3, fBly);
   AddFrame(fHframe4, fBly);

   // Set main frame name, map sub windows (buttons), initialize layout
   // algorithm via Resize() and map main frame
   SetWindowName("Triple Slider Demo");
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();

   Double_t ymax = 10000;

   T->Draw("L.prl1.e:L.prl2.e>>h1(500,0,1500,500,0,1500)","","colz");
   h1->GetYaxis()->SetTitle("L.prl1.e");
   h1->GetXaxis()->SetTitle("L.prl2.e");
   h1->GetYaxis()->SetTitleOffset(1.55);

   //fFitFcn = new TF1("fFitFcn", "TMath::LogNormal(x, [0], [1], [2])*1000", 0, 5);
   //fFitFcn = new TF1("fFitFcn", "300", 0, 3000); //Horizontal line at y=300.
   fFitFcn = new TF1("fFitFcn", "[0] * x + [1]", 0, 1500);

   fFitFcn->SetRange(0.0, 1500);
   fFitFcn->SetParameters(-0.8, 630.0);
   fFitFcn->SetMinimum(1.0e-3);
   fFitFcn->SetMaximum(10000.0);
   fFitFcn->SetLineColor(kRed);
   fFitFcn->SetLineWidth(1);
   fFitFcn->Draw("SAME");

   //Set slider positions.
   fHslider1->SetPosition(0.0,3000);
   fHslider1->SetPointerPosition(630.0);

   fHslider2->SetPosition(-2.0,0.0);
   fHslider2->SetPointerPosition(-0.80);

   //Print slider positions to boxes.
   sprintf(buf, "%.3f", fHslider1->GetMinPosition());
   fTbh1->Clear();
   fTbh1->AddText(0, buf);
   sprintf(buf, "%.3f", fHslider1->GetPointerPosition());
   fTbh2->Clear();
   fTbh2->AddText(0, buf);
   sprintf(buf, "%.3f", fHslider1->GetMaxPosition());
   fTbh3->Clear();
   fTbh3->AddText(0, buf);

   sprintf(buf, "%.3f", fHslider2->GetMinPosition());
   fTbh4->Clear();
   fTbh4->AddText(0, buf);
   sprintf(buf, "%.3f", fHslider2->GetPointerPosition());
   fTbh5->Clear();
   fTbh5->AddText(0, buf);
   sprintf(buf, "%.3f", fHslider2->GetMaxPosition());
   fTbh6->Clear();
   fTbh6->AddText(0, buf);

   pad1->cd(2);
   pad1->cd(2)->SetLogy();

   T->Draw("L.cer.asum_c>>h2(500,0,3000)");
   h2->GetYaxis()->SetTitle("Counts");
   h2->GetXaxis()->SetTitle("L.cer.asum_c");
   h2->SetMaximum(ymax);
   h2->GetYaxis()->SetTitleOffset(1.3);
}

//______________________________________________________________________________
TTripleSliderDemo::~TTripleSliderDemo()
{
   // Clean up

   Cleanup();
}

//______________________________________________________________________________
void TTripleSliderDemo::CloseWindow()
{
   // Called when window is closed via the window manager.

   delete this;
}

//______________________________________________________________________________
void TTripleSliderDemo::DoText(const char * /*text*/)
{
   // Handle text entry widgets.

   TGTextEntry *te = (TGTextEntry *) gTQSender;
   Int_t id = te->WidgetId();

   switch (id) 
     {
      case HId1:
         fHslider1->SetPosition(atof(fTbh1->GetString()),
                                fHslider1->GetMaxPosition());
         break;
      case HId2:
         fHslider1->SetPointerPosition(atof(fTbh2->GetString()));
         break;
      case HId3:
         fHslider1->SetPosition(fHslider1->GetMinPosition(),
                                atof(fTbh1->GetString()));
         break;
      default:
         break;
     }

   switch (id) 
     {
      case HId4:
         fHslider2->SetPosition(atof(fTbh4->GetString()),
                                fHslider2->GetMaxPosition());
         break;
      case HId5:
         fHslider2->SetPointerPosition(atof(fTbh5->GetString()));
         break;
      case HId6:
         fHslider2->SetPosition(fHslider2->GetMinPosition(),
                                atof(fTbh6->GetString()));
         break;
      default:
         break;
     }
   //fFitFcn->SetParameters(fHslider1->GetPointerPosition(), 0, 1);
   //fFitFcn->SetRange(fHslider1->GetMinPosition(), fHslider1->GetMaxPosition());
   //histo1->Draw();
   //fFitFcn->Draw("SAME");
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}

//______________________________________________________________________________
void TTripleSliderDemo::DoSlider()
{ 
   // Handle slider widgets.
   char buf[32];

   //Update text boxes.
   sprintf(buf, "%.3f", fHslider1->GetMinPosition());
   fTbh1->Clear();
   fTbh1->AddText(0, buf);
   fTeh1->SetCursorPosition(fTeh1->GetCursorPosition());
   fTeh1->Deselect();
   gClient->NeedRedraw(fTeh1);

   sprintf(buf, "%.3f", fHslider1->GetPointerPosition());
   fTbh2->Clear();
   fTbh2->AddText(0, buf);
   fTeh2->SetCursorPosition(fTeh2->GetCursorPosition());
   fTeh2->Deselect();
   gClient->NeedRedraw(fTeh2);

   sprintf(buf, "%.3f", fHslider1->GetMaxPosition());
   fTbh3->Clear();
   fTbh3->AddText(0, buf);
   fTeh3->SetCursorPosition(fTeh3->GetCursorPosition());
   fTeh3->Deselect();
   gClient->NeedRedraw(fTeh3);

   sprintf(buf, "%.3f", fHslider2->GetMinPosition());
   fTbh4->Clear();
   fTbh4->AddText(0, buf);
   fTeh4->SetCursorPosition(fTeh4->GetCursorPosition());
   fTeh4->Deselect();
   gClient->NeedRedraw(fTeh4);

   sprintf(buf, "%.3f", fHslider2->GetPointerPosition());
   fTbh5->Clear();
   fTbh5->AddText(0, buf);
   fTeh5->SetCursorPosition(fTeh5->GetCursorPosition());
   fTeh5->Deselect();
   gClient->NeedRedraw(fTeh5);

   sprintf(buf, "%.3f", fHslider2->GetMaxPosition());
   fTbh6->Clear();
   fTbh6->AddText(0, buf);
   fTeh6->SetCursorPosition(fTeh6->GetCursorPosition());
   fTeh6->Deselect();
   gClient->NeedRedraw(fTeh6);

   //fFitFcn->SetParameters(fHslider1->GetPointerPosition(), 0, 1);
   //fFitFcn->SetRange(fHslider1->GetMinPosition(), fHslider1->GetMaxPosition());

   Double_t ymax = 10000;

   pad1->cd(1);

   T->Draw("L.prl1.e:L.prl2.e>>h1(500,0,1500,500,0,1500)","","colz");
   h1->GetYaxis()->SetTitle("L.prl1.e");
   h1->GetXaxis()->SetTitle("L.prl2.e");
   h1->GetYaxis()->SetTitleOffset(1.55);

   Int_t cermin = 300;
   Double_t slidervalue = 0;
   Double_t slope = fHslider2->GetPointerPosition();
   slidervalue = fHslider1->GetPointerPosition();

   //Set line parameters (slope, y-int).
   fFitFcn->SetParameters(slope, slidervalue);
   //cout<<"Y = "<<fFitFcn->GetParameter(0)<<" * X +  "<<fFitFcn->GetParameter(1)<<endl;

   fFitFcn->Draw("SAME");

   pad1->cd(2);

   //TCut ct1 = Form("L.cer.asum_c>%f",slidervalue);
   TCut ct1 = Form("L.prl1.e > %f * L.prl2.e + %f",slope,slidervalue);

   T->Draw("L.cer.asum_c>>h2(500,0,3000)",ct1);
   h2->GetYaxis()->SetTitle("Counts");
   h2->GetXaxis()->SetTitle("L.cer.asum_c");
   h2->SetMaximum(ymax);
   h2->GetYaxis()->SetTitleOffset(1.3);

   //Set slider min and max to adjust the X axis range of second histogram.
   h2->GetXaxis()->SetRangeUser(fHslider1->GetMinPosition(), fHslider1->GetMaxPosition());

   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}

//______________________________________________________________________________
void TTripleSliderDemo::HandleButtons()
{
   // Handle different buttons.

   TGButton *btn = (TGButton *) gTQSender;
   Int_t id = btn->WidgetId();

   switch (id) {
      case HCId1:
         fHslider1->SetConstrained(fCheck1->GetState());
         break;
      case HCId2:
         fHslider1->SetRelative(fCheck2->GetState());
         break;
      default:
         break;
   }
}


void Shower_Cut_3He(Int_t run)
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

  new TTripleSliderDemo(); 
}
