void hist(){
  char filename[100];
  TH1F *kansho = new TH1F("kansho","",75,0.,75.);
  std::cout<<"t"<<std::endl;
  for(int i=0; i<75; ++i){
    sprintf(filename, "test_009_%03d.root",i);
    auto f = TFile::Open(filename);
    auto hist = dynamic_cast<TH1*>(f->Get("ADC_HIGH_0"));
    if(hist==nullptr){
      std::cout<<"nullhist "<<i<<std::endl;
      f->Close();
      delete f;
      continue;
    }
    double mean = hist->GetMean();
    //hist->Fit("gaus","","",810,870);
    //auto func = hist->GetFunction("gaus");
    //double p0 = func->Integral(810,870)/10000.;
    //std::cout<<p0<<std::endl;
    //double value = -std::log(p0);
    kansho->SetBinContent(i+1, mean);
    f->Close();
    delete f;
  }
  kansho->Draw();
}
