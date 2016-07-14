# DxLib_own_create_window

http://detail.chiebukuro.yahoo.co.jp/qa/question_detail/q10161640440

のためのテストrepo

## require

DxLib and [DxLibEx](https://github.com/Nagarei/DxLibEx) is required.

## How to build

1. 環境変数``DXLIB_ROOT``と``DXLIBEX_ROOT``を設定する  
ex.)``DXLIB_ROOT``を``D:\lib\DxLib_VC\プロジェクトに追加すべきファイル_VC用``に  
``DXLIBEX_ROOT``を``C:\Users\yumetodo\Documents\git\DxLibEx``に
2. ``DxLib_own_create_window.sln``を立ち上げ、ビルド→ソリューションのビルドでビルドする

## Link
- **[c# - Handling AeroSnap message in WndProc - Stack Overflow](http://stackoverflow.com/questions/9321549/handling-aerosnap-message-in-wndproc)**
- [DXlibのウィンドウサイズを可変に、Aeroスナップとか最大化も - Qiita](http://qiita.com/hypertis/items/507c624dc5314b042cfa#%E6%A8%99%E6%BA%96%E3%81%AE%E5%A0%B4%E5%90%88)
- [winapi - Win32 App (Aero): Prevent dialog move - Stack Overflow](http://stackoverflow.com/questions/16169818/win32-app-aero-prevent-dialog-move)
- [WINDOWPOS structure (Windows)](https://msdn.microsoft.com/en-us/library/windows/desktop/ms632612(v=vs.85).aspx)
- [WM_WINDOWPOSCHANGING message (Windows)](https://msdn.microsoft.com/en-us/library/windows/desktop/ms632653(v=vs.85).aspx)
- [Win32API ディスプレイモニターの情報を取得する - s-kitaの日記](http://d.hatena.ne.jp/s-kita/20130502/1367485535)
- [MonitorFromWindow 関数 | MSDN](https://msdn.microsoft.com/ja-jp/library/cc410476.aspx)