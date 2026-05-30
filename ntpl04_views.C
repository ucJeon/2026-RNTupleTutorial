/// \file ntpl04_views.C
/// \brief [04] 뷰 기반 부분 읽기 + I/O 메트릭
///
/// 실행: root -l -b -q ntpl04_views.C     (data01.root 또는 data02.root 필요)
/// ROOT 6.36+
///
/// 핵심 동기: 컬럼나 포맷의 최대 장점 = "필요한 필드만 읽기".
///   RNTupleView<T> 는 지정한 필드의 페이지만 디스크에서 읽어온다.
///   넓은 스키마에서 1~2개 컬럼만 쓰는 분석일수록 read throughput 이득이 크다.

#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <ROOT/RNTupleReader.hxx>

#include <TRandom3.h>
#include <memory>

using ROOT::RNTupleModel;
using ROOT::RNTupleWriter;
using ROOT::RNTupleReader;

static void MakeWideFile()
{
   // 일부러 필드를 많이 둔 "넓은" ntuple 생성
   auto model = RNTupleModel::Create();
   auto a = model->MakeField<float>("a");
   auto b = model->MakeField<float>("b");
   auto c = model->MakeField<float>("c");
   auto d = model->MakeField<double>("d");
   auto e = model->MakeField<int>("e");

   auto writer = RNTupleWriter::Recreate(std::move(model), "Events", "data04.root");
   for (int i = 0; i < 100000; ++i) {
      *a = gRandom->Gaus(); *b = gRandom->Gaus(); *c = gRandom->Gaus();
      *d = gRandom->Gaus(); *e = gRandom->Integer(100);
      writer->Fill();
   }
}

static void PartialRead()
{
   auto reader = RNTupleReader::Open("Events", "data04.root");

   // 메트릭 측정 ON (반드시 루프 전에)
   reader->EnableMetrics();

   // 필드 'a' 하나만 본다 → 'a' 컬럼 페이지만 디스크에서 읽힘
   auto va = reader->GetView<float>("a");
   double sum = 0;
   for (auto i : reader->GetEntryRange())
      sum += va(i);

   Printf("<a> = %.4f", sum / reader->GetNEntries());

   // 디컴프레션 시간, 읽은 바이트 등 I/O 메트릭 덤프
   reader->PrintInfo(ROOT::ENTupleInfo::kMetrics);
}

void ntpl04_views()
{
   MakeWideFile();
   PartialRead();
}
