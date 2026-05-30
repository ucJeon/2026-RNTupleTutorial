/// \file ntpl01_hello.C
/// \brief [01] Hello RNTuple — 스칼라 필드 write/read 최소 예제
///
/// 실행: root -l -b -q ntpl01_hello.C
/// ROOT 6.36+ (production 네임스페이스 ROOT::)

#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <ROOT/RNTupleReader.hxx>

#include <TRandom3.h>
#include <memory>

// --- 6.36+ production 네임스페이스 ---
//     6.34 이하라면 아래를 ROOT::Experimental::... 로 교체
using ROOT::RNTupleModel;
using ROOT::RNTupleWriter;
using ROOT::RNTupleReader;

static void Write()
{
   // 1) 스키마(모델) 생성 — TTree 자체에 해당
   auto model = RNTupleModel::Create();

   // 2) 필드 추가 — TBranch 에 해당. MakeField<T> 는 std::shared_ptr<T> 반환.
   //    이 포인터가 default entry 의 메모리 위치에 연결됨 → 값만 바꿔 Fill.
   auto pt   = model->MakeField<float>("pt");
   auto njet = model->MakeField<int>("njet");

   // 3) Writer 생성: 모델 소유권 이전 + ntuple 이름 + 파일명
   //    Recreate = 파일 덮어쓰기 (Append = 기존 파일에 추가)
   auto writer = RNTupleWriter::Recreate(std::move(model), "Events", "data01.root");

   // 4) 이벤트 루프: 필드 포인터에 값 대입 후 Fill
   for (int i = 0; i < 1000; ++i) {
      *pt   = gRandom->Gaus(50, 10);
      *njet = gRandom->Integer(8);
      writer->Fill();
   }
   // writer 소멸 시 자동으로 buffer flush + 파일 close (명시적 Close 불필요)
}

static void Read()
{
   // Open(ntupleName, fileName): 디스크 메타데이터로부터 모델 자동 추론
   auto reader = RNTupleReader::Open("Events", "data01.root");

   // 스키마 + 엔트리 수 요약 출력
   reader->PrintInfo();
   Printf("nEntries = %llu", reader->GetNEntries());

   // 0번 엔트리를 JSON 형태로 미리보기
   reader->Show(0);

   // 컬럼 접근은 View 가 권장(부분 읽기에 유리; 04번에서 심화)
   auto viewPt = reader->GetView<float>("pt");
   double sum = 0;
   for (auto i : reader->GetEntryRange())   // 전체 엔트리 인덱스 순회
      sum += viewPt(i);
   Printf("<pt> = %.3f", sum / reader->GetNEntries());
}

void ntpl01_hello()
{
   Write();
   Read();
}
