/// \file ntpl02_vector.C
/// \brief [02] 컬렉션 필드 — 이벤트당 가변 개수 (std::vector)
///
/// 실행: root -l -b -q ntpl02_vector.C
/// ROOT 6.36+
///
/// 포인트: std::vector<T> 는 내부적으로 (offset 컬럼 + payload 컬럼) 으로 분해 저장된다.
///         offset/payload 분리가 RNTuple 의 zero-copy 매핑·압축 효율의 핵심.

#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <ROOT/RNTupleReader.hxx>

#include <TRandom3.h>
#include <vector>
#include <memory>

using ROOT::RNTupleModel;
using ROOT::RNTupleWriter;
using ROOT::RNTupleReader;

static void Write()
{
   auto model = RNTupleModel::Create();

   // 가변길이 필드: 이벤트마다 크기가 다른 jet pT 목록
   auto jetPt  = model->MakeField<std::vector<float>>("jetPt");
   auto jetEta = model->MakeField<std::vector<float>>("jetEta");
   auto nLep   = model->MakeField<std::uint32_t>("nLep");   // 스칼라 동반

   auto writer = RNTupleWriter::Recreate(std::move(model), "Events", "data02.root");

   for (int i = 0; i < 2000; ++i) {
      int njet = gRandom->Integer(10);     // 0~9개

      jetPt->clear();                       // 각 엔트리 전 반드시 clear
      jetEta->clear();
      for (int j = 0; j < njet; ++j) {
         jetPt->emplace_back(gRandom->Gaus(60, 20));
         jetEta->emplace_back(gRandom->Uniform(-2.5, 2.5));
      }
      *nLep = gRandom->Integer(4);

      writer->Fill();
   }
}

static void Read()
{
   auto reader = RNTupleReader::Open("Events", "data02.root");
   reader->PrintInfo();

   // 컬렉션도 View 로 통째 읽기 가능 (View 가 std::vector<float> 를 반환)
   auto vJetPt = reader->GetView<std::vector<float>>("jetPt");

   long totalJets = 0; double sumPt = 0;
   for (auto i : reader->GetEntryRange()) {
      const auto &jets = vJetPt(i);         // i번 엔트리의 vector 참조
      totalJets += jets.size();
      for (float p : jets) sumPt += p;
   }
   Printf("총 jet 수 = %ld,  평균 jetPt = %.3f", totalJets, sumPt / totalJets);

   reader->Show(0);   // 컬렉션이 JSON 배열로 보임
}

void ntpl02_vector()
{
   Write();
   Read();
}
