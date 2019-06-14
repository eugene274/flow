#ifdef __CLING__

R__LOAD_LIBRARY(libQnCorrections.so)
R__LOAD_LIBRARY(libBase.so)

#endif

using namespace std;
using namespace Qn;

void save_graphs(TString inputFileName = "corr_2.root")
{	
  TFile *fIn = TFile::Open (inputFileName);
  // declare correlations
  DataContainer<Stats> *u_east_Q_west_XX, *u_west_Q_east_XX, *Q_east_Q_west_XX, *u_east_Q_west_YY, *u_west_Q_east_YY, *Q_east_Q_west_YY;
  
  // get correlations from file
  fIn->GetObject("pion_east_pT_BBCwest_XX", u_east_Q_west_XX);
  fIn->GetObject("pion_west_pT_BBCeast_XX", u_west_Q_east_XX);
  fIn->GetObject("BBCwest_BBCeast_XX", Q_east_Q_west_XX);
  fIn->GetObject("pion_east_pT_BBCwest_YY", u_east_Q_west_YY);
  fIn->GetObject("pion_west_pT_BBCeast_YY", u_west_Q_east_YY);
  fIn->GetObject("BBCwest_BBCeast_YY", Q_east_Q_west_YY);
  
  // compute resolutions 
  auto Rx = Sqrt (*Q_east_Q_west_XX);
  auto Ry = Sqrt (*Q_east_Q_west_YY);
  
  // divide correlations by reslution
  auto v1x_east = *u_east_Q_west_XX / Rx * sqrt(2);
  auto v1x_west = *u_west_Q_east_XX / Rx * sqrt(2);
  auto v1y_east = *u_east_Q_west_YY / Ry * sqrt(2);
  auto v1y_west = *u_west_Q_east_YY / Ry * sqrt(2);
  
  // group into wider centrality bins 
  v1x_west = v1x_west.Rebin({"Centrality", {0,10,40,80}});
  v1y_west = v1y_west.Rebin({"Centrality", {0,10,40,80}});
  v1x_east = v1x_east.Rebin({"Centrality", {0,10,40,80}});
  v1y_east = v1y_east.Rebin({"Centrality", {0,10,40,80}});
  
  // unite west and east measurements with some weights
  auto v1x = v1x_west * 0.99 + v1x_east * 1.01;
  auto v1y = v1y_west * 0.99 + v1y_east * 1.01;
  
  // unite x and y components
  auto v1 = v1x + v1y;
  
  // obtain graphs for resolutions
  TGraphAsymmErrors *gRx = DataContainerHelper::ToTGraph (Rx);
  TGraphAsymmErrors *gRy = DataContainerHelper::ToTGraph (Ry);
  
  //obtain multigraph for v1 in all centrality bins
  TMultiGraph *mgV = DataContainerHelper::ToTMultiGraph (v1, "Centrality");
  
  // drawing
  mgV->SetTitle("v_{1};p_{T} (GeV/c);v_{1}");
  TMultiGraph *mgR = new TMultiGraph ();
  mgR->SetTitle("R_{1};Centrality, %;R_{1}");
  mgR->Add(gRx);
  mgR->Add(gRy);
  gRx->SetTitle("R_{1,x}");
  gRy->SetTitle("R_{1,y}");
  
  TCanvas *c = new TCanvas();
  c->Divide(2,1);
  c->cd(1);
  mgR->Draw("apl pmc plc");
  gPad->BuildLegend();
  c->cd(2);
  mgV->Draw("apl pmc plc");
//  mgV->GetYaxis()->SetRangeUser(-0.2,0.2);
  gPad->BuildLegend();
}