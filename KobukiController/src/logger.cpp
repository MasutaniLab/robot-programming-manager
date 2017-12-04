//���V�X�e�������u���{�b�g�v���O���~���O�v
//2016/6/8 ���J �۔�
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

//�T�v�F���O�t�@�C���ւ̏o�͏���
//�����F�Ȃ�
//�߂�l�F�Ȃ�
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
        cerr << "���O�f�B���N�g���쐬���s" << endl;
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
        cerr << "���O�t�@�C���I�[�v�����s" << endl;
        exit(1);
    }
    ofs << fixed;
}

//�T�v�F �����ŗ^����ꂽ�������O�t�@�C���֏o��
//�����F pos ���݈ʒu�Cbum �o���p�Z���T�̏�ԁCkey �L�[���́Cvel ���x�w��
//�߂�l�F�Ȃ�
void logger(const Orthogonal &pos, const Bumper &bum, const Velocity &vel)
{
  double t = getTime();
  ofs << t << " " 
    << pos.x << " " << pos.y << " " << pos.q << " "
    << bum.right << " " << bum.center << " " << bum.left << " "
    << vel.v << " " << vel.w << endl;
}

//�T�v�F���O�t�@�C���ւ̏o�͏I��
//�����F�Ȃ�
//�߂�l�F�Ȃ�
void loggerTerminate()
{
  if (ofs.is_open()) {
    ofs.close();
  }
}

