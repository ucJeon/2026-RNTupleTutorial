# RNTuple 기초 튜토리얼 세트 (ROOT 6.36+ LTS)

ROOT의 차세대 컬럼나 I/O인 **RNTuple**을 TTree 경험 위에서 익히기 위한 실행형 매크로 세트.
모든 코드는 **production 네임스페이스 `ROOT::`** (6.36 LTS 이후) 기준.

## 0. 시작 전 — 버전·네임스페이스 체크

```bash
root --version          # 6.36.00 이상인지 확인
```

- **6.36+** : `ROOT::RNTupleModel`, `ROOT::RNTupleWriter`, `ROOT::RNTupleReader` (production)
- **6.34 이하** : 위 클래스들이 `ROOT::Experimental::` 에 있음 → `using` 한 줄만 바꾸면 동일 코드 동작
- 단, `RNTupleInspector`, `RNTupleProcessor`, `RNTupleParallelWriter` 는 6.36에서도 아직 `ROOT::Experimental::`

> sscc.uos 클러스터의 ROOT 빌드 버전을 먼저 확인하고, 6.34 이하라면 각 매크로 상단의 `using` 블록을 Experimental 쪽으로 교체.

## 1. 실행 방법

```bash
root -l -b -q ntpl01_hello.C            # 인터프리터 실행
root -l -b -q 'ntpl03_object.C+'         # ACLiC 컴파일 필요 (사용자 클래스 → dictionary)
```

순서대로 실행하면 `data*.root` 파일이 생성되고 뒤 튜토리얼이 그것을 읽음.
처음부터 끝까지 한 번에:

```bash
for f in ntpl00 ntpl01 ntpl02 ntpl04 ntpl05 ntpl06 ntpl07; do root -l -b -q ${f}*.C; done
root -l -b -q 'ntpl03_object.C+'
```

## 2. 커리큘럼

| # | 파일 | 주제 | 핵심 API |
|---|---|---|---|
| 00 | `ntpl00_tfile_reminder.C` | TFile/TTree 리마인드 + 대응표 | `TFile::Open/Get/Write`, `TTree::Branch/Fill` |
| 01 | `ntpl01_hello.C` | 스칼라 필드 write/read | `RNTupleModel::Create`, `MakeField`, `Recreate`, `Open` |
| 02 | `ntpl02_vector.C` | 가변길이 컬렉션 | `MakeField<std::vector<T>>` |
| 03 | `ntpl03_object.C` | 사용자 정의 클래스 (ACLiC) | `MakeField<MyStruct>` + dictionary |
| 04 | `ntpl04_views.C` | 뷰 기반 부분 읽기 + 메트릭 | `GetView`, `GetEntryRange`, `EnableMetrics` |
| 05 | `ntpl05_rdataframe.C` | RDataFrame 연동 | `RDataFrame("ntpl","f.root")` |
| 06 | `ntpl06_ttree2rntuple.C` | TTree → RNTuple 변환 | `TTreeReader` → `RNTupleWriter` |
| 07 | `ntpl07_inspector.C` | 저장 효율·압축 검사 | `RNTupleInspector::Create` |

## 3. TTree ↔ RNTuple 대응 (한눈에)

| TTree | RNTuple |
|---|---|
| `TTree` | `RNTuple` / 스키마는 `RNTupleModel` |
| `TBranch` | `RField` (템플릿, 타입-세이프) |
| `TBasket` | `RPage` |
| (cluster) | `RCluster` |
| `TTreeReader` | `RNTupleReader` |
| `TTreeReaderValue<T>` | `RNTupleView<T>` |
| `TTreeCache` | `RClusterPool` |
| `tree->Fill()` | `writer->Fill()` |
| `tree->GetEntry(i)` | `reader->LoadEntry(i)` |

