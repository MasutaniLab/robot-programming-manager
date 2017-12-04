// -*- C++ -*-
/*!
 * @file  CraneplusWrapper.cpp
 * @brief ${rtcParam.description}
 * @date $Date$
 *
 * $Id$
 */

#define _USE_MATH_DEFINES
#include <iostream>
#include <iomanip>
#include <cmath>
#include "CraneplusWrapper.h"
using namespace std;

//�萔
const double HandMargin = 0.04; // [m]
const double L1 = 0.0714; // [m]
const double L2 = 0.0830; // [m]
const double L3 = 0.0935; // [m]
const double L4 = 0.0600 + HandMargin; // [m]
const double pos2rad[5] = {0.00511327, -0.00511327, 0.00511327, 0.00511327, 0.00511327};
const double posoffset[5] = {512, 512+307.2, 512, 512, 512};
const double rps2speed = 1024/114.0*60.0/(2*M_PI); //rad/s -> moving speed
const double handClosed = 0.645; //[rad]
const double handOpen = -0.6; //[rad]

const int StatusBitMoving = 1;
const int StatusBitExecuting = 2;
const int StatusBitCommandError = 4;
const int StatusBitLowerError = 8;

//�v���g�^�C�v�錾
void directKinematics(double &x, double &y, double &z, double &pitch, double q[4]);
bool inverseKinematics(double q[4], double x, double y, double z, double pitch, const int mode);
double normalizeAngle(double x);

// Module specification
// <rtc-template block="module_spec">
static const char* cranepluswrapper_spec[] =
  {
    "implementation_id", "CraneplusWrapper",
    "type_name",         "CraneplusWrapper",
    "description",       "${rtcParam.description}",
    "version",           "1.0.0",
    "vendor",            "MasutaniLab",
    "category",          "Arm",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
CraneplusWrapper::CraneplusWrapper(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_armTipTargetIn("armTipTarget", m_armTipTarget),
    m_armJointTargetIn("armJointTarget", m_armJointTarget),
    m_presentPositionIn("presentPosition", m_presentPosition),
    m_movingIn("moving", m_moving),
    m_armTipOut("armTip", m_armTip),
    m_armJointOut("armJoint", m_armJoint),
    m_goalPositionOut("goalPosition", m_goalPosition),
    m_movingSpeedOut("movingSpeed", m_movingSpeed),
    m_armStatusOut("armStatus", m_armStatus)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
CraneplusWrapper::~CraneplusWrapper()
{
}



RTC::ReturnCode_t CraneplusWrapper::onInitialize()
{
  cout << "CraneplusWrapper::onInitialize()" << endl;
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("armTipTarget", m_armTipTargetIn);
  addInPort("armJointTarget", m_armJointTargetIn);
  addInPort("presentPosition", m_presentPositionIn);
  addInPort("moving", m_movingIn);
  
  // Set OutPort buffer
  addOutPort("armTip", m_armTipOut);
  addOutPort("armJoint", m_armJointOut);
  addOutPort("goalPosition", m_goalPositionOut);
  addOutPort("movingSpeed", m_movingSpeedOut);
  addOutPort("armStatus", m_armStatusOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>

  cout << fixed << setprecision(4);

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CraneplusWrapper::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CraneplusWrapper::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CraneplusWrapper::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t CraneplusWrapper::onActivated(RTC::UniqueId ec_id)
{
  cout << "CraneplusWrapper::onActivated()" << endl;

  m_armTip.data.length(5);
  m_armJoint.data.length(m_NUM_ACTUATOR);
  m_goalPosition.data.length(m_NUM_ACTUATOR); 
  m_movingSpeed.data.length(m_NUM_ACTUATOR); 

  //�����̖ڕW
  for (int i=0; i<m_NUM_ACTUATOR; i++) {
    m_jointTarget[i] = 0;
  }
  m_jointTarget[1] = M_PI/2; //�������
  m_jointTarget[4] = handOpen;
  m_time = 1.0;
  m_newTarget = false;
  m_movingPrev = false;
  m_errorPrev = false;
  m_rangeError = false;
  m_stopCount = 0;
  m_state = Start;
  m_statePrev = Stop;
  m_timeStart = coil::gettimeofday(); 
  m_ikError = false;
  m_newCommand = false;
  m_init = true;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t CraneplusWrapper::onDeactivated(RTC::UniqueId ec_id)
{
  cout << "CraneplusWrapper::onDeactivated()" << endl;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t CraneplusWrapper::onExecute(RTC::UniqueId ec_id)
{
  //�֐ߎw��
  if (m_armJointTargetIn.isNew()) {
    cout << "m_armJointTargetIn.isNew()" << endl;
    m_armJointTargetIn.read();
    for (int i = 0; i<m_NUM_ACTUATOR; i++) {
      m_jointTarget[i] = m_armJointTarget.data[i];
      cout << i << " " << m_jointTarget[i] << endl;
    }
    m_time =  m_armJointTarget.data[m_NUM_ACTUATOR];
    cout << "mitme: " << m_time << endl;
    if (m_time < 0) {
      m_time = 0 ;
    }
    m_newTarget = true;
  }

  //���w��
  if (m_armTipTargetIn.isNew()) {
    m_armTipTargetIn.read();
    cout << "m_armTipTargetIn.isNew()" << endl;
    double x     = m_armTipTarget.data[0];
    double y     = m_armTipTarget.data[1];
    double z     = m_armTipTarget.data[2];
    double pitch = m_armTipTarget.data[3];
    double hand  = m_armTipTarget.data[4];
    m_time  = m_armTipTarget.data[5];

    cout << "x: " << x << endl;
    cout << "y: " << y << endl;
    cout << "z: " << z << endl;
    cout << "pitch: " << pitch << endl;
    cout << "hand: " << hand << endl;
    cout << "m_time: " << m_time << endl;

    if (m_time < 0) {
      m_time = 0;
    }
    //�t�^���w�������C�֐߂̖ڕW�����߂�
    m_ikError = false;
    if (pitch < -M_PI || pitch > M_PI) {
      m_ikError = true;
    } else {
      int mode = 1;
#if 0
      int mode = 0;

      if (z < 0.125) { //z���W�������ł���ΕI����ɂ���i�֐�2�𕉂Ɂj
        mode =1;
      }
#endif
      m_ikError = inverseKinematics(m_jointTarget,x,y,z,pitch,mode);
    }
    cout << "m_ikError: " << m_ikError << endl;

    //���݊p�x���l�����Ċ֐ߑ��x�̏�������߂�
    if (!m_ikError) {
      if (hand < 0) {
        hand = 0;
      } else if (hand > 1) {
        hand = 1;
      }
      m_jointTarget[4] = hand*(handOpen-handClosed) + handClosed;
      cout << "m_jointTarget:" << endl;
      for (int i = 0; i<m_NUM_ACTUATOR; i++) {
        cout << i << ": " << m_jointTarget[i] << endl;
      }
      m_newTarget = true;
    }
  }

  //Dynamixel�̎w�߂ɕϊ�
  if (m_newTarget) { //�V�����w�ߒl�������
    m_rangeError = false;
    for (int i = 0; i<m_NUM_ACTUATOR; i++) {
      //�p�x������\���ɕϊ�
      int p = int(m_jointTarget[i]/pos2rad[i]+posoffset[i]+0.5);
      if (p < 0 || 1023 < p) {
        m_rangeError = true;
      }
      m_goalPosition.data[i] = p;
     //���x���v�Z
      double dq;
      if (m_time == 0) {
        dq = 0;
      } else {
        dq = (m_jointTarget[i] - m_jointCurrent[i])/m_time; //[rad/s]
        //cout << i << " dq: " << dq << " " << m_jointTarget[i] << " " << m_jointCurrent[i] << endl;
      }
      //���x������\���ɕϊ�
      int s = abs(dq*rps2speed); //0�̏ꍇ�͍ō����x
      if (s < 0 || 1023 < s) {
        m_rangeError = true;
      }
      m_movingSpeed.data[i] = s;
    }
    cout << "m_goalPosition.data:" << endl;
    for (int i = 0; i<m_NUM_ACTUATOR; i++) {
      cout << i << ": " << m_goalPosition.data[i] << endl;
    }
    cout << "m_movingSpeed.dat:" << endl;
    for (int i = 0; i<m_NUM_ACTUATOR; i++) {
      cout << i << ": " << m_movingSpeed.data[i] << endl;
    }
    //�o�̓|�[�g�ɏ�������
    if (m_rangeError) {
      cout << "m_rangeError" << endl;
    } else {
      m_goalPositionOut.write();
      m_movingSpeedOut.write();
      m_newCommand = true;
    }
    m_newTarget = false;
  }

  //Dynamixel����̓��͂ɑΉ�
  if (m_presentPositionIn.isNew()) {
    m_presentPositionIn.read();
    //�p�x������\������rad��
    for (int i = 0; i<m_NUM_ACTUATOR; i++) {
      //Dynamixel����̃f�[�^���s�ǂȏꍇ��z��
      int p = m_presentPosition.data[i];
      if (p < 0 || 1023 < p) {
        cout << "m_presentPosition.data[" << i << "] : " << p << " �͈͊O�I" << endl;
        p = 512;
      }
      m_jointCurrent[i] = (p-posoffset[i])*pos2rad[i];
      m_armJoint.data[i] = m_jointCurrent[i];
    }
    //�o�̓|�[�g�ɏ�������
    m_armJointOut.write();
    //���^���w�v�Z
    double x, y, z, pitch;
    directKinematics(x, y, z, pitch, m_jointCurrent);
    m_armTip.data[0] = x;
    m_armTip.data[1] = y;
    m_armTip.data[2] = z;
    m_armTip.data[3] = pitch;
    m_armTip.data[4] = (m_jointCurrent[4]-handClosed)/(handOpen-handClosed);
    //�o�̓|�[�g�ɏ�������
    m_armTipOut.write();
  }

  if (m_movingIn.isNew()) {
    m_movingIn.read();
    bool moving = false;
    bool error = false;
    for (int i = 0; i<m_NUM_ACTUATOR; i++) {
      //cout << m_moving.data[i] << ",";
      if (m_moving.data[i] == 1) {
        moving = true;
      } else if (m_moving.data[i] == 3) {
        error = true;
      }
    }
    //cout << endl;
    if (moving != m_movingPrev) {
      if (moving) {
        cout << "���쒆" << endl;
      } else {
        cout << "��~��" << endl;
      }
    }
    m_movingPrev = moving;
    if (error != m_errorPrev) {
      if (error) {
        cout << "���ʂŃG���[����" << endl;
      } else {
        cout << "���ʂŃG���[�Ȃ�" << endl;
      }
    }
    m_errorPrev = error;

    //��ԑJ��
    bool stateChanged = false;
    if (m_state != m_statePrev) {
      stateChanged = true;
    }
    m_statePrev = m_state;
    if (m_ikError || m_rangeError) {
      m_state = CommandError;
      cout << "--> CommandError" << endl;
    }
    if (error) {
      m_state = LowerError;
      cout << "--> LowerError" << endl;
    }
    //cout << "m_state: " << m_state << endl;
    if (m_state == Start) {
      if (stateChanged) {
        m_timeStart = coil::gettimeofday();
        m_newCommand = false;
      }
      if (moving) {//�����o������
        m_state = Execute;
        cout << "Start --> Execute" << endl;
      } else {
        double elapsedTime = coil::gettimeofday() - m_timeStart;
        //cout << "elapsedTime: " << elapsedTime << endl;
        if (elapsedTime > 1.0) {//�J�n��������Ԏ~�܂��Ă�����
          m_state = Stop;
          cout << "Start --> Stop" << endl;
        }
      }
    } else if (m_state == Execute) {
      if (!moving) {//�~�܂�����
        m_state = Stop;
        cout << "Execute  --> Stop" << endl;
      }
    } else if (m_state == Stop) {
      if (m_newCommand) {//�V�����w�߂��󂯂���
        m_state = Start;
        cout << "Stop  --> Start" << endl;
      }
    } else if (m_state == CommandError) {
      if (m_newCommand) {//�V�����w�߂��󂯂���
        m_state = Start;
        cout << "CommandError  --> Start" << endl;
      }
    } else if (m_state == LowerError) {
      if (!error && !moving) {//���ʂ̃G���[���Ȃ��Ȃ�~�܂��Ă�����
        m_state = Stop;
        cout << "LowerError  --> Stop" << endl;
      }
    }

    //armStatus�|�[�g�̏o�͂̌���
    m_armStatus.data = 0;
    if (moving) {
      m_armStatus.data |= StatusBitMoving;
    }
    if (m_state == Start || m_state == Execute) {
      m_armStatus.data |= StatusBitExecuting;
    } else if (m_state == CommandError) {
      m_armStatus.data |= StatusBitCommandError;
    } else if (m_state == LowerError) {
      m_armStatus.data |= StatusBitLowerError;
    }
    //cout << "m_armStatus.data: " << m_armStatus.data << endl;
    m_armStatusOut.write();
  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CraneplusWrapper::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t CraneplusWrapper::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t CraneplusWrapper::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t CraneplusWrapper::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t CraneplusWrapper::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

// 4��]�֐߃A�[���̏��^���w
// x,y,z,pitch  ���̈ʒu�ƕ����D�y�o�́z
// q[4]  q[0]�`q[3]����1�`4�֐߂̊p�x�D�t�^���w�̉��D�y���́z
// �߂�l �Ȃ�
void
directKinematics(double &x, double &y, double &z, double &pitch, double q[4])
{
  double q1 = q[0];
  double q2 = q[0];
  double q3 = q[0];
  double q4 = q[0];
  double c1 = cos(q1);
  double s1 = sin(q1);
  double c2 = cos(q2);
  double s2 = sin(q2);
  double c23 = cos(q2+q3);
  double s23 = sin(q2+q3);
  double c234 = cos(q2+q3+q4);
  double s234 = sin(q2+q3+q4);
  x = c1*(L2*c2 + + L3*c23 + L4*c234);
  y = s1*(L2*c2 + + L3*c23 + L4*c234);
  z = L1 + L2*s2 + + L3*s23 + L4*s234;
  pitch = q2+q3+q4;
}

// 4��]�֐߃A�[���̋t�^���w
// q[4]  q[0]�`q[3]����1�`4�֐߂̊p�x�D�t�^���w�̉��D�y�o�́z
// x,y,z,pitch  ���̈ʒu�ƕ����D�y���́z
// mode   �t�^���w�̉��̎�ށD0: ��3�֐߂����C1: ��3�֐߂����D
// �߂�l �t�^���w��������ꍇ��false�C�����Ȃ��ꍇ��true�D
bool 
inverseKinematics(double q[4], double x, double y, double z, double pitch, const int mode)
{
  double q1, q2, q3, q4;
  q1 = atan2(y,x);
  double r = sqrt(x*x+y*y);
  double rr = r - L4*cos(pitch);
  double zz = z - L4*sin(pitch) - L1;
  double d = sqrt(rr*rr+zz*zz);
  double cq3 = (d*d - L2*L2 - L3*L3)/(2*L2*L3);
  if ( abs(cq3) > 1.0 ) return true;
  double phi = atan2(zz, rr);
  double psi = acos((d*d + L2*L2 - L3*L3)/(2*L2*d));
  if ( mode == 0 ) {
    q2 = normalizeAngle(phi-psi); 
    q3 = acos(cq3);
  } else {
    q2 = normalizeAngle(phi+psi); 
    q3 = -acos(cq3);
  }
  q4 = normalizeAngle(pitch-q2-q3);
  q[0] = q1;
  q[1] = q2;
  q[2] = q3;
  q[3] = q4;

  return false;
}

// �����̊p�x��-�΁`+�΂̊Ԃɐ��K���������̂�Ԃ�
double
normalizeAngle(double x)
{
  if      (x<-M_PI) x += M_PI*2*(double)((int)(-x/(2*M_PI)+0.5));
  else if (x> M_PI) x -= M_PI*2*(double)((int)( x/(2*M_PI)+0.5));
  return x;
}

extern "C"
{
 
  void CraneplusWrapperInit(RTC::Manager* manager)
  {
    coil::Properties profile(cranepluswrapper_spec);
    manager->registerFactory(profile,
                             RTC::Create<CraneplusWrapper>,
                             RTC::Delete<CraneplusWrapper>);
  }
  
};


