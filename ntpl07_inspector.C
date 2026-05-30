/// \file ntpl07_inspector.C
/// \brief [07] 저장 효율 검사 + 압축 설정 튜닝
///
/// 실행: root -l -b -q ntpl07_inspector.C
/// ROOT 6.36+
///
/// 주의: RNTupleInspector 는 6.36 에서도 아직 ROOT::Experimental:: 네임스페이스.
///       (Reader/Writer/Model 과 달리 production 전이 진행 중)

#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriter.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <ROOT/RNTupleInspector.hxx>
#include <ROOT/RNTuple.hxx>          // on-disk anchor ROOT::RNTuple

#include <TFile.h>
#include <TRandom3.h>
#include <vector>
#include <memory>

using ROOT::RNTupleModel;
using ROOT::RNTupleWriter;
using ROOT::Experimental::RNTupleInspector;   // <-- Experimental 유지

// 동일 데이터를 압축 설정만 바꿔 두 번 쓴다
static void WriteWithCompression(const char *fname, int comp)
{
   auto model = RNTupleModel::Create();
   auto x = model->MakeField<double>("x");
   auto v = model->MakeField<std::vector<float>>("v");

   // 쓰기 옵션: 압축 알고리즘*100 + 레벨
   //   1=ZLIB, 2=LZMA, 4=LZ4, 5=ZSTD   (예: 505 = ZSTD level5, 0 = 무압축)
   ROOT::RNTupleWriteOptions opts;
   opts.SetCompression(comp);

   auto writer = RNTupleWriter::Recreate(std::move(model), "Events", fname, opts);
   for (int i = 0; i < 50000; ++i) {
      *x = gRandom->Gaus(100, 5);
      v->clear();
      for (int j = 0, n = gRandom->Integer(8); j < n; ++j)
         v->emplace_back(gRandom->Gaus());
      writer->Fill();
   }
}

static void Inspect(const char *fname)
{
   // on-disk anchor(ROOT::RNTuple) 를 TFile 에서 회수 → Inspector 에 전달
   std::unique_ptr<TFile> f(TFile::Open(fname, "READ"));
   auto *anchor = f->Get<ROOT::RNTuple>("Events");

   auto insp = RNTupleInspector::Create(anchor);

   Printf("[%s]  압축=%lu B  비압축=%lu B  압축률=%.2f",
          fname,
          (unsigned long)insp->GetCompressedSize(),
          (unsigned long)insp->GetUncompressedSize(),
          insp->GetCompressionFactor());
}

void ntpl07_inspector()
{
   WriteWithCompression("data07_none.root", 0);     // 무압축
   WriteWithCompression("data07_zstd.root", 505);   // ZSTD level 5

   Inspect("data07_none.root");
   Inspect("data07_zstd.root");

   // ---------------------------------------------------------------
   // 추가 튜닝 포인트 (RNTupleWriteOptions):
   //   - SetApproxZippedClusterSize(...) : 클러스터(=병렬 쓰기 단위) 목표 크기
   //   - SetMaxUnzippedPageSize(...)     : 페이지 크기 (TBasket 대응)
   //   대용량 쓰기/원격 읽기 throughput 에 직접 영향. 데이터 특성 보며 조정.
   // ---------------------------------------------------------------
}
