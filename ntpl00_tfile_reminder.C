/// \file ntpl00_tfile_reminder.C
/// \brief [00] TFile / TTree 리마인드 — RNTuple 들어가기 전 기본 I/O 상기
///
/// 실행: root -l -b -q ntpl00_tfile_reminder.C
/// ROOT 6.36+ (네임스페이스 무관, 전통 TTree API)
///
/// 목적: RNTuple의 write/read 패턴이 TTree와 어떻게 대응되는지 비교 기준을 만든다.

void ntpl00_tfile_reminder()
{
   // ---------------------------------------------------------------
   // (A) WRITE — TFile 열고 TTree에 Branch 걸고 Fill
   // ---------------------------------------------------------------
   {
      // RECREATE: 기존 파일 덮어쓰기. (그 외 NEW/READ/UPDATE)
      TFile f("legacy.root", "RECREATE");

      // TTree 는 반드시 열린 TFile 컨텍스트 안에서 생성 (디렉터리 소유)
      TTree tree("T", "demo tree");

      // 브랜치에 연결할 변수 — 주소를 SetBranchAddress 와 동일하게 등록
      float pt;
      int   njet;
      tree.Branch("pt",   &pt,   "pt/F");    // leaflist 로 타입 명시 (/F float, /I int)
      tree.Branch("njet", &njet, "njet/I");

      for (int i = 0; i < 1000; ++i) {
         pt   = gRandom->Gaus(50, 10);
         njet = gRandom->Integer(8);
         tree.Fill();                        // 현재 변수 스냅샷을 한 엔트리로 기록
      }

      tree.Write();   // TTree 객체를 파일 키로 저장
      f.Close();      // 버퍼 flush + 파일 닫기 (스코프 종료로도 됨)
   }

   // ---------------------------------------------------------------
   // (B) READ — TFile::Open → Get<TTree> → TTreeReader
   // ---------------------------------------------------------------
   {
      // TFile::Open 은 static, TFile* 반환 (로컬/원격 xrootd 모두 처리)
      std::unique_ptr<TFile> f(TFile::Open("legacy.root", "READ"));
      if (!f || f->IsZombie()) { Error("ntpl00", "open 실패"); return; }

      // 키 이름으로 객체 회수. Get<T> 가 타입-세이프한 현대식.
      auto *tree = f->Get<TTree>("T");

      // TTreeReader: GetEntry/SetBranchAddress 보다 안전한 현대 읽기 인터페이스
      TTreeReader reader(tree);
      TTreeReaderValue<float> pt(reader, "pt");
      TTreeReaderValue<int>   njet(reader, "njet");

      double sum = 0; long n = 0;
      while (reader.Next()) {           // 엔트리 순회
         sum += *pt; ++n;              // *pt 로 현재 엔트리 값 접근
      }
      Printf("[TTree] entries=%lld  <pt>=%.3f", tree->GetEntries(), sum / n);
   }

   // ---------------------------------------------------------------
   // 대응 관계 (다음 튜토리얼부터 RNTuple 로 동일 작업 반복)
   //   TFile + TTree            -> RNTupleModel + RNTupleWriter (파일 자동 관리)
   //   tree.Branch(&var,...)    -> model->MakeField<T>("name")  (shared_ptr 반환)
   //   tree.Fill()              -> writer->Fill()
   //   TTreeReader / Value<T>   -> RNTupleReader / GetView<T>
   //   tree->GetEntry(i)        -> reader->LoadEntry(i)
   // ---------------------------------------------------------------
}
