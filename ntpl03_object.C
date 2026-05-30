/// \file ntpl03_object.C
/// \brief [03] 사용자 정의 클래스 저장 — RField<T> + dictionary
///
/// 실행: root -l -b -q 'ntpl03_object.C+'      <-- 끝의 '+' 필수!
///
/// 왜 '+' (ACLiC) 가 필요한가:
///   RNTuple 의 RField<T> 는 사용자 타입 T 를 컬럼으로 매핑하기 위해 ROOT 타입시스템에
///   등록된 dictionary 가 있어야 한다. ACLiC( '+' )가 이 매크로를 컴파일하면서
///   rootcling 으로 struct 의 dictionary 를 자동 생성해 준다.
///   (순수 인터프리터 실행 root -l -b -q ntpl03_object.C 은 dictionary 부재로 실패할 수 있음)
///
/// 실무 분석에서는 별도 헤더 + LinkDef.h 로 dictionary 를 미리 만들어 라이브러리화한다.

#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <ROOT/RNTupleReader.hxx>

#include <TRandom3.h>
#include <vector>
#include <memory>

using ROOT::RNTupleModel;
using ROOT::RNTupleWriter;
using ROOT::RNTupleReader;

// --- 사용자 정의 레코드 (ACLiC 가 dictionary 자동 생성) ---
struct Particle {
   float px = 0.f, py = 0.f, pz = 0.f;
   int   pdgId = 0;

   float Pt() const { return std::sqrt(px * px + py * py); }
};

static void Write()
{
   auto model = RNTupleModel::Create();

   // 단일 객체 필드 + 객체의 vector 필드 둘 다 시연
   auto met       = model->MakeField<Particle>("met");                 // 객체 1개
   auto particles = model->MakeField<std::vector<Particle>>("particles"); // 객체 컬렉션

   auto writer = RNTupleWriter::Recreate(std::move(model), "Events", "data03.root");

   for (int i = 0; i < 1000; ++i) {
      met->px = gRandom->Gaus(0, 30);
      met->py = gRandom->Gaus(0, 30);
      met->pdgId = 0;

      particles->clear();
      int n = gRandom->Integer(6);
      for (int j = 0; j < n; ++j) {
         Particle p;
         p.px = gRandom->Gaus(0, 50);
         p.py = gRandom->Gaus(0, 50);
         p.pz = gRandom->Gaus(0, 80);
         p.pdgId = (gRandom->Rndm() < 0.5) ? 11 : 13;   // e / mu
         particles->emplace_back(p);
      }
      writer->Fill();
   }
}

static void Read()
{
   auto reader = RNTupleReader::Open("Events", "data03.root");
   reader->PrintInfo();   // 객체 필드는 하위필드(px,py,...)로 펼쳐져 표시됨

   auto vMet  = reader->GetView<Particle>("met");
   auto vPart = reader->GetView<std::vector<Particle>>("particles");

   double sumMet = 0; long nMu = 0;
   for (auto i : reader->GetEntryRange()) {
      sumMet += vMet(i).Pt();
      for (const auto &p : vPart(i)) if (p.pdgId == 13) ++nMu;
   }
   Printf("<MET pT> = %.3f,  muon 수 = %ld", sumMet / reader->GetNEntries(), nMu);
}

void ntpl03_object()
{
   Write();
   Read();
}
