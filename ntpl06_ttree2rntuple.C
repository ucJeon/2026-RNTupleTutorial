/// \file ntpl06_ttree2rntuple.C
/// \brief [06] TTree → RNTuple 변환 (수동 루프)
///
/// 실행: root -l -b -q ntpl06_ttree2rntuple.C
/// ROOT 6.36+
///
/// 기존 TTree 데이터를 RNTuple 로 옮기는 가장 투명한 방법: TTreeReader 로 읽어
/// RNTupleWriter 로 다시 쓴다. 브랜치↔필드 대응이 코드에 그대로 드러나 학습에 좋다.
///
/// 실무 단축 경로:
///   - ROOT::Experimental::RNTupleImporter  (전용 변환 도구, 스키마 자동 복제)
///   - RDataFrame + Snapshot(kRNTuple)       (필터/리매핑 동반 변환)
/// 대용량/스키마 보존이 목적이면 위 도구를 권장.

#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <ROOT/RNTupleReader.hxx>

#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TRandom3.h>
#include <memory>

using ROOT::RNTupleModel;
using ROOT::RNTupleWriter;
using ROOT::RNTupleReader;

static void MakeLegacyTree()
{
   TFile f("legacy06.root", "RECREATE");
   TTree t("T", "legacy");
   float pt, eta; int q;
   t.Branch("pt", &pt, "pt/F");
   t.Branch("eta", &eta, "eta/F");
   t.Branch("q", &q, "q/I");
   for (int i = 0; i < 5000; ++i) {
      pt = gRandom->Gaus(40, 8); eta = gRandom->Uniform(-2.4, 2.4);
      q = gRandom->Rndm() < 0.5 ? -1 : 1;
      t.Fill();
   }
   t.Write(); f.Close();
}

static void Convert()
{
   // 입력 TTree 열기
   std::unique_ptr<TFile> fin(TFile::Open("legacy06.root", "READ"));
   auto *tree = fin->Get<TTree>("T");

   TTreeReader rdr(tree);
   TTreeReaderValue<float> in_pt(rdr, "pt");
   TTreeReaderValue<float> in_eta(rdr, "eta");
   TTreeReaderValue<int>   in_q(rdr, "q");

   // 출력 RNTuple 모델 — 브랜치와 1:1 대응되는 필드
   auto model = RNTupleModel::Create();
   auto pt  = model->MakeField<float>("pt");
   auto eta = model->MakeField<float>("eta");
   auto q   = model->MakeField<int>("q");

   auto writer = RNTupleWriter::Recreate(std::move(model), "Events", "data06.root");

   // 엔트리 단위 복사: TTreeReader.Next() ↔ writer->Fill()
   while (rdr.Next()) {
      *pt  = *in_pt;
      *eta = *in_eta;
      *q   = *in_q;
      writer->Fill();
   }
   // writer 소멸 시 저장
}

static void Verify()
{
   auto reader = RNTupleReader::Open("Events", "data06.root");
   Printf("변환 결과 nEntries = %llu", reader->GetNEntries());
   reader->PrintInfo();
}

void ntpl06_ttree2rntuple()
{
   MakeLegacyTree();
   Convert();
   Verify();
}
