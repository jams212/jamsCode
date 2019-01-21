#ifndef REDNET_UREDNET_H
#define REDNET_UREDNET_H

#define REDNET_VERSION "2.0.0 bate"

#include "uevent_queue.h"
#include "usingleton.h"

namespace rednet
{

class utimer;
class uworks;
class uenv;
class ucontexts;
class umodules;
class usignal;
class uglobal_queue;
namespace server
{
class ugroup;
}

class usystem : public usingleton<usystem>
{
  static constexpr int default_worker = 1;
  struct deploy
  {
    int workerNumber;
    int serverGroupID;
    const char *daemon;
    const char *modulePath;
    const char *bootstrap;
    const char *logService;
    const char *logFile;
    const char *luaPath;
    const char *luaCPath;
    const char *luaService;
    bool proFile;
  };

  enum workerSerial
  {
    WS_WORKER = 0,
    WS_MAIN = 1,
    WS_TIMER = 2,
    WS_SOCKET = 3,
  };

  friend void
  startup(const char *config_file);
  friend bool loadDeploy(const char *filename);
  friend void initLogger();
  friend void initBootstrap();
  friend class uenv;
  friend class ucall;
  friend class utimer;
  friend class ucontext;
  friend class uevent_queue;

public:
  usystem() : workArray(NULL), 
              envHandle(NULL), 
              ctxArray(NULL), 
              modArray(NULL), 
              qeHandle(NULL), 
              clockArray(NULL), 
              serverGroup(NULL), 
              hubsig(NULL) {}
  ~usystem() {}

private:
  void init();
  void destory();

private:
  void signalHub();
  eq_ptr dispatchEvent(eq_ptr q, int weight);

private:
  int workTimer(void *param);
  int workLogic(void *param);

private:
  uworks *workArray;
  uenv *envHandle;
  ucontexts *ctxArray;
  umodules *modArray;
  uglobal_queue *qeHandle;

private:
  utimer *clockArray;
  server::ugroup *serverGroup;
  usignal *hubsig;

public:
  deploy deployHandle;
};

void startup(const char *config_file);
} // namespace rednet

#define SYSTEM_CALL rednet::usystem::instance()
#define EQ_CALL SYSTEM_CALL->qeHandle
#define ENV_CALL SYSTEM_CALL->envHandle
#define UCTXS_CALL SYSTEM_CALL->ctxArray
#define UMODS_CALL SYSTEM_CALL->modArray
#define WORKS_CALL SYSTEM_CALL->workArray
#define CLOCK_CALL SYSTEM_CALL->clockArray
#define REMOTE_CALL SYSTEM_CALL->serverGroup
#define DEPLOY_H SYSTEM_CALL->deployHandle

#endif