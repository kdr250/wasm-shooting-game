# wasm-shooting-game
SDL2 + CMake + Emscripten のサンプルです。SDL2を使って作ったゲームがWebブラウザ上でも動作します。

## 環境構築
### 前提条件
- [Visual Studio Code](https://code.visualstudio.com/)
- [C++ コンパイラ](https://code.visualstudio.com/docs/languages/cpp#_install-a-compiler)
- [CMake](https://cmake.org/download/)
- [Ninja](https://ninja-build.org/)

### インストール
- 以下を参考に、C++拡張機能をインストールする。
  - [Visual Studio Code 拡張機能をインストールする](https://learn.microsoft.com/ja-jp/vcpkg/get_started/get-started-vscode?pivots=shell-powershell#3---install-visual-studio-code-extensions)
- 以下を参考に、vcpkgをインストールして環境変数を設定し、依存関係のインストールを行う。
  - [vcpkg を設定する](https://learn.microsoft.com/ja-jp/vcpkg/get_started/get-started-vscode?pivots=shell-powershell#1---set-up-vcpkg)
  - [環境変数を設定する > VCPKG_ROOT環境変数を構成します](https://learn.microsoft.com/ja-jp/vcpkg/get_started/get-started-vscode?pivots=shell-powershell#4---set-up-environment-variables)
  - [マニフェスト モードでのパッケージのインストール](https://learn.microsoft.com/ja-jp/vcpkg/concepts/manifest-mode#install-manifest-mode)<br>※実行環境に合わせて `--triplet=XXX` オプションを指定すること
- 以下を参考に、Emscriptenをインストールして有効化する。
  - [emscripten - Download and install](https://emscripten.org/docs/getting_started/downloads.html)

## ビルド方法
### ネイティブの場合
[Build a project](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/how-to.md#build-a-project) を参考にして、ビルドして実行。

### Webの場合
1. プロジェクト直下で `emcmake cmake -B build-web` を実行。
2. `cmake --build build-web` を実行。
3. `python -m http.server -d build-web` を実行してWebサーバー起動。
4. ブラウザで `http://localhost:8000/main.html` にアクセスして確認。

## 参考にしたURL
- [SDL Wiki > Emscripten](https://wiki.libsdl.org/SDL2/README/emscripten)
