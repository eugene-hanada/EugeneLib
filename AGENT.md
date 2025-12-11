# EugeneLib — AGENT

## 概要
このリポジトリは日本語でのやり取りを基本とします。  
EugeneLib は C++23 を前提としたゲーム開発用のクロスプラットフォーム（主に Windows／Android）向けのライブラリ群で、グラフィックス（DirectX12 / Vulkan）、サウンド、システム抽象（ウィンドウ・入力）、ユーティリティを含みます。ビルドは __CMake__（最小 3.20）で管理され、オプションで ImGui、Vulkan、DirectX12、サウンド、ネットワーク等を切り替えられます。外部依存は内部サブモジュール／サブディレクトリ（fmt, mimalloc, VulkanMemoryAllocator 等）で管理されています。

主な設計方針
- 名前空間: Eugene
- 標準ライブラリ（STL）の積極的活用（std::vector, std::string_view, std::optional, std::span 等）
- API と実装の分離（ヘッダは Include/*、実装は Source/*）
- プラットフォーム別実装を条件付きコンパイルで切り替え（例: Windows/Android 用 System クラス）
- RAII とムーブセマンティクスを多用（ComPtr / vk::Unique* / std::move）
- パフォーマンスを意識した低レイヤー設計（mimalloc や VMA の使用を想定）
- 例外ポリシー: 初期化時やリソース確保など致命的なエラーは例外で表現する。フレーム毎の処理やユーザー入力などランタイムの回復可能なエラーは戻り値／ステータスで扱い、ゲームループ内で例外を投げない設計を推奨する。
- ゲーム向けを想定しているため、動的なメモリ割り当ては最小限に抑える。

## リポジトリ構成（抜粋）
- Include/… — パブリックヘッダ、プラットフォーム別ヘッダ（Windows/Android）、Graphics/Vulkan/DirectX12 関連
- Source/… — 実装
- Include/ThirdParty/… — サードパーティ（fmt, mimalloc, imgui, VMA 等）
- Sample/… — サンプルアプリ
- Test/… — doctest を用いたテスト

## ビルド（短縮）
- ルートの __CMakeLists.txt__ を使用。
- 主な CMake オプション:
  - EUGENE_USE_WINDOWS (既定 ON)
  - EUGENE_USE_ANDROID
  - EUGENE_USE_VULKAN / EUGENE_USE_DX12
  - EUGENE_USE_IMGUI
  - EUGENE_BUILD_SAMPLE / EUGENE_BUILD_DOCTEST / EUGENE_BUILD_SOUND / EUGENE_BUILD_NETWORK

## コーディング規約（リポジトリに合わせた推奨）
このリポジトリの既存コードスタイルを踏襲し、以下を守る。

命名規則
- 型・クラス名: PascalCase（例: System, Graphics）
- メソッド名: PascalCase（例: Create, GetMouse）
- 列挙型: enum class を使用し、列挙子は PascalCase（例: WindowMode::Borderless）
- 名前空間: Eugene
- メンバ変数: trailing underscore（例: windowSize_, isActive_）
- 定数: PascalCase（static constexpr）を推奨（既存コードに合わせる）

インターフェース設計
- public API はヘッダに定義、実装は Source/ に置く。
- 値渡しは小型 POD のみ、それ以外は const T& または std::span/std::string_view を使用。
- 所有を明確にする（Raw pointer は非所有、所有は unique_ptr / ComPtr / vk::Unique* を使用）。
- コピー禁止は明示（= delete）し、ムーブを提供する（move-only なリソース管理）。

言語機能・品質
- 標準: C++23
- 常に const / noexcept を適切に付与（特にデストラクタや軽量ゲッタ）。
- nullptr と std::optional を適切に使用。
- std::span を API に使い、コピーコストを抑える。
- マジックナンバー禁止、named constants を使用する。
- エラー処理は例外か戻り値かで一貫性を保つ（既存コードは戻り値 + Final() 形式が多い）。

ヘッダとインクルード
- ヘッダガードは既存に合わせて `#pragma once` を使用。
- インクルード順（推奨）:
  1. 対象ファイルのヘッダ（自ヘッダ）
  2. C/C++ 標準ヘッダ
  3. サードパーティヘッダ（glm, fmt 等）
  4. プロジェクト内ヘッダ
- 余計なインクルードは前方宣言で置き換え、ビルド時間を削減する。

スタイル（既存コードに合わせる）
- インデントはタブ（既存ソースに合わせる）。
- ブレースは同じ行（例: void Foo() { ... }）を多用する慣習を踏襲。
- 行長は 120 文字以内を目安に折り返す。
- テンプレートや長い宣言は複数行で整列。
- コメントは Doxygen スタイルを基本とし、public API には必須。内容はドキュメント代わりになるように具体的に。

リソース管理
- DirectX 用は Microsoft::WRL::ComPtr、Vulkan 用は vk::Unique* / VMA の UniqueAllocation を使用。
- Final()/~Destructor を組み合わせた明確なクリーンアップを行う。
- 移動コンストラクタ／移動代入を実装してリソースの安全な受け渡しを保証する。

テスト／CI（簡易提案）
- 現在テストは doctest（Test/ フォルダ）だが、今回は考慮外。
- 可能であれば CI で __cmake --build__ と __ctest__（テスト有効時）を走らせる。
- 一貫したコード整形のためルートに `.clang-format` を置き、CI で整形チェックを行うことを推奨する。

レビュー・PR チェックリスト（短く）
- ビルドが通る（Debug/Release）
- 新たな外部依存は README に明記
- public API 変更はヘッダのドキュメントを更新
- メモリリーク・リソース解放を確認（Final(), move semantics）

## Copilot / 自動支援について
- Copilot にリポジトリ固有の短い指示を読み込ませる場合はルートではなく `/.github/copilot-instructions.md` を併設してください。  
- Visual Studio で Copilot のカスタム指示を自動適用するには該当設定を有効にしてください（例: __Enable custom instructions to be loaded from .github/copilot-instructions.md files and added to requests__）。  
- ただしコピペ可能な機密情報はこの種のファイルに記載しないでください。

## 注意点・設計上の留意事項
- Windows と Android（GameActivity）で API が大きく異なるため、プラットフォーム抽象層は慎重に扱うこと。
- グラフィックス機能は DirectX と Vulkan で実装差分が多く、共通インターフェース（Graphics, ImageResource, RenderTargetViews 等）を壊さないようにする。
- グローバル状態やシングルトン（DynamicSingleton）の濫用はテスト性／再利用性を下げるため、必要最小限に留める。

---
（この AGENT.md はリポジトリの現在のソース/ヘッダ構成を踏まえ、実装スタイルに沿ったコーディング規約と運用上の指針を短くまとめたものです。README.md からのリンクを推奨します。）