//情報システム実験「ロボットプログラミング」
//2016/6/8 升谷 保博
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <time.h>


#if defined(_WINDOWS) || defined(WIN32) //Windows
  #include <Windows.h>
  #include <shlwapi.h>
#elif LINUX //Linux
  #include <sys/stat.h>
#endif
#include <cstdlib>
#include "ise-robot.h"
#include "util.h"
#include "logger.h"
using namespace std;

ofstream ofs;

//概要：ログファイルへの出力準備
//引数：なし
//戻り値：なし
void loggerInitialize()
{
    time_t timer;
    struct tm *date;
    char str[15];

    timer = time(NULL);
    date = localtime(&timer);
    strftime(str, sizeof(str), "%Y%m%d%H%M%S", date);
    string head = ISE_ROBOT_SOURCE;
    if ( head.substr(head.size()-4) == ".cpp" ) {
      head.erase(head.size()-4);
    }
    string logfile = "log/" + head + '-' + str + ".txt";
    cout << "logfile: " << logfile << endl;
#if defined(_WINDOWS) || defined(WIN32) //Windows
    if (::PathIsDirectory("log") == 0) {
      if (CreateDirectory("log", NULL) == 0) {
        cerr << "ログディレクトリ作成失敗" << endl;
        exit(1);
      }
    }
#elif LINUX //Linux
  struct stat sb;
  if (stat("log", &sb) == -1) {
   mkdir("./log",S_IEXEC|S_IWRITE|S_IREAD);
  }
#endif  
    ofs.open(logfile.c_str());
    if (!ofs) {
        cerr << "ログファイルオープン失敗" << endl;
        exit(1);
    }
    ofs << fixed;
}

//概要： 引数で与えられた情報をログファイルへ出力
//引数： pos 現在位置，bum バンパセンサの状態，key キー入力，vel 速度指令
//戻り値：なし
void logger(const Orthogonal &pos, const Bumper &bum, const Velocity &vel)
{
  double t = getTime();
  ofs << t << " " 
    << pos.x << " " << pos.y << " " << pos.q << " "
    << bum.right << " " << bum.center << " " << bum.left << " "
    << vel.v << " " << vel.w << endl;
}

//概要：ログファイルへの出力終了
//引数：なし
//戻り値：なし
void loggerTerminate()
{
  if (ofs.is_open()) {
    ofs.close();
  }
}

