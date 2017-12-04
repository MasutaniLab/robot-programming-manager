// -*- C++ -*-
/*!
 * @file  KobukiController.cpp
 * @brief ${rtcParam.description}
 * @date $Date$
 *
 * $Id$
 */

#include <iostream>
#if defined(_WINDOWS) || defined(WIN32) //Windows
  #include <conio.h>
#endif
#include "util.h"
#include "draw.h"
#include "logger.h"
#include "KobukiController.h"
using namespace std;

// Module specification
// <rtc-template block="module_spec">
static const char* kobukicontroller_spec[] =
  {
    "implementation_id", "KobukiController",
    "type_name",         "KobukiController",
    "description",       "${rtcParam.description}",
    "version",           "1.0.0",
    "vendor",            "MasutaniLab",
    "category",          "Mobile Robot",
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
KobukiController::KobukiController(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_currentPoseIn("currentPose", m_currentPose),
    m_bumperIn("bumper", m_bumper),
    m_targetVelocityOut("targetVelocity", m_targetVelocity)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
KobukiController::~KobukiController()
{
}



RTC::ReturnCode_t KobukiController::onInitialize()
{
  cout << "KobukiController::onInitialize()" << endl;
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("currentPose", m_currentPoseIn);
  addInPort("bumper", m_bumperIn);
  
  // Set OutPort buffer
  addOutPort("targetVelocity", m_targetVelocityOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t KobukiController::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiController::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiController::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t KobukiController::onActivated(RTC::UniqueId ec_id)
{
  cout << "KobukiController::onActivated()" << endl;
  getTimeInitialize();
  drawInitialize();
  loggerInitialize();
  setup();
  m_vel.v = 0;
  m_vel.w = 0;
  return RTC::RTC_OK;
}


RTC::ReturnCode_t KobukiController::onDeactivated(RTC::UniqueId ec_id)
{
  cout << "KobukiController::onDeactivated()" << endl;
  drawTerminate();
  loggerTerminate();
  return RTC::RTC_OK;
}


RTC::ReturnCode_t KobukiController::onExecute(RTC::UniqueId ec_id)
{
  const double VMax = 0.3;
  const double WMax = 1.0;

  if (m_bumperIn.isNew() && m_currentPoseIn.isNew()) {
    m_bumperIn.read();
    Bumper bum;
    bum.right  = m_bumper.data[0];
    bum.center = m_bumper.data[1];
    bum.left   = m_bumper.data[2];

    m_currentPoseIn.read();
    Orthogonal pos;
    pos.x = m_currentPose.data.position.x;
    pos.y = m_currentPose.data.position.y;
    pos.q = m_currentPose.data.heading;

    //cout << pos.x << " " << pos.y << " " << pos.q << endl;
    static double pt = 0;
    double ct = getTime();
    //cout << ct - pt << endl;
    pt = ct;

    int key = drawInkey();

    m_vel = loop(pos, bum, key);

    //cout << "v: " << m_vel.v << "w: " << m_vel.w << endl;
    if ( m_vel.v > VMax ) {
      m_vel.v = VMax;
    } else if (m_vel.v < -VMax ) {
      m_vel.v = -VMax;
    }
    if ( m_vel.w > WMax ) {
      m_vel.w = WMax;
    } else if (m_vel.w < -WMax ) {
      m_vel.w = -WMax;
    }

    m_targetVelocity.data.vx = m_vel.v;
    m_targetVelocity.data.vy = 0;
    m_targetVelocity.data.va = m_vel.w-m_vel.v*0.2;
    m_targetVelocityOut.write();

    draw(pos, bum, m_vel);
    logger(pos, bum, m_vel);
  }

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t KobukiController::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiController::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiController::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiController::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t KobukiController::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void KobukiControllerInit(RTC::Manager* manager)
  {
    coil::Properties profile(kobukicontroller_spec);
    manager->registerFactory(profile,
                             RTC::Create<KobukiController>,
                             RTC::Delete<KobukiController>);
  }
  
};


