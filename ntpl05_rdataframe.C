/// \file ntpl05_rdataframe.C
/// \brief [05] RDataFrame 로 RNTuple 분석
///
/// 실행: root -l -b -q ntpl05_rdataframe.C     (data02.root 필요 → 먼저 ntpl02 실행)
/// ROOT 6.36+
///
/// 핵심: RDataFrame 생성자에 (ntupleName, fileName) 만 주면 RNTuple 인지 TTree 인지
///       자동 감지한다. 분석 코드는 소스 포맷과 무관하게 동일.

#include <ROOT/RDataFrame.hxx>
#include <TH1D.h>
#include <TFile.h>

void ntpl05_rdataframe()
{
   // RNTuple "Events" 를 RDF 소스로 — TTree 와 동일한 생성자 형태
   ROOT::RDataFrame df("Events", "data02.root");

   // 컬럼 정의 / 필터 / 집계 — lazy 그래프, 결과 객체 접근 시 한 번에 실행
   auto df2 = df.Define("nJet", "jetPt.size()");

   auto nTotal   = df2.Count();
   auto nWithJet = df2.Filter("nJet > 0", "has jet").Count();
   auto meanNJet = df2.Mean("nJet");

   // 6.36 에서 컬렉션 필드는 자동으로 ROOT::RVec 로 노출됨 → 벡터 연산 가능
   auto hLeadPt = df2.Filter("nJet > 0")
                     .Define("leadPt", "Max(jetPt)")     // RVec 헬퍼
                     .Histo1D({"hLeadPt", "leading jet pT;pT;events", 50, 0, 150}, "leadPt");

   Printf("entries=%llu,  jet>=1 비율=%.3f,  <nJet>=%.3f",
          *nTotal, double(*nWithJet) / *nTotal, *meanNJet);

   // 히스토그램을 결과 파일에 저장
   TFile out("data05_hist.root", "RECREATE");
   hLeadPt->Write();
   out.Close();

   // ---------------------------------------------------------------
   // 참고: 분석 결과를 다시 RNTuple 로 저장 (6.36 신규)
   //   ROOT::RDF::RSnapshotOptions opt;
   //   opt.fOutputFormat = ROOT::RDF::ESnapshotOutputFormat::kRNTuple;
   //   df2.Snapshot("Events", "skim.root", {"jetPt", "nJet"}, opt);
   // (enum 경로는 패치 버전에 따라 다를 수 있으니 root 문서 확인)
   // ---------------------------------------------------------------
}
