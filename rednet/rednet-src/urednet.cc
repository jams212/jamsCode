#include "urednet.h"
#include "ucontext.h"
#include "udaemon.h"
#include "uenv.h"
#include "uglobal.h"
#include "ugroup.h"
#include "uini.h"
#include "umodule.h"
#include "utimer.h"
#include "uwork.h"
#include "usignal.h"
#include <assert.h>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IS_ABORT                                     \
    if (rednet::uglobal::instance()->contextEmpty()) \
        break;

namespace rednet
{

void usystem::init()
{
    uglobal::instance()->init();

    workArray = new uworks();

    envHandle = new uenv();

    ctxArray = new ucontexts();

    modArray = new umodules();

    qeHandle = new uglobal_queue();
    //#######################################################################
    clockArray = new utimer();

    serverGroup = new server::ugroup();


    hubsig    = new usignal(SA_RESTART, SIGHUP, std::bind(&usystem::signalHub, this));

    assert(workArray &&
           envHandle &&
           ctxArray &&
           modArray &&
           qeHandle &&
           clockArray &&
           serverGroup);
}

void usystem::destory()
{
    if (clockArray)
    {
        delete clockArray;
        clockArray = NULL;
    }

    if (workArray)
    {
        delete workArray;
        workArray = NULL;
    }

    if (envHandle)
    {
        delete envHandle;
        envHandle = NULL;
    }

    if (ctxArray)
    {
        delete ctxArray;
        ctxArray = NULL;
    }

    if (modArray)
    {
        delete modArray;
        modArray = NULL;
    }

    if (qeHandle)
    {
        delete qeHandle;
        qeHandle = NULL;
    }

    if (serverGroup)
    {
        delete serverGroup;
        serverGroup = NULL;
    }
    
    if (hubsig)
    {
        delete hubsig;
        hubsig = NULL;
    }

    uglobal::instance()->exit();
}

void usystem::signalHub()
{
    uevent e;
    e.source = 0;
    e.data = NULL;
    e.type = EP_OPER;
    e.sz = 0;

    uint32_t id = UCTXS_CALL->convertId(DEPLOY_H.logService);
    if (id == 0)
        return;
    ucall::sendEvent(id, &e);
}

int usystem::workTimer(void *param)
{
    uglobal::instance()->bindWorker(WS_TIMER);
    for (;;)
    {
        CLOCK_CALL->tick();
        //msocket->UpdateTime(Timer::Instance()->Now());
        IS_ABORT
        WORKS_CALL->wakeup(WORKS_CALL->getCount() - 1);
        usleep(2500);
        hubsig->wait();
    }

    //exit socket server
    WORKS_CALL->stop();

    return 0;
}

int usystem::workLogic(void *param)
{
    eq_ptr q_ptr = nullptr;
    uglobal::instance()->bindWorker(WS_WORKER);
    while (!workArray->isShutdown())
    {
        q_ptr = dispatchEvent(q_ptr, -1);
        if (q_ptr == nullptr)
            workArray->wait();
    }
    return 0;
}

eq_ptr usystem::dispatchEvent(eq_ptr q, int weight)
{
    if (q == nullptr)
    {
        q = qeHandle->pop();
        if (q == nullptr)
            return nullptr;
    }

    uint32_t id = q->id;
    ctx_ptr ptr = ctxArray->grab(id);

    if (ptr == nullptr)
    {
        return qeHandle->pop();
    }

    int i, n = 1;
    uevent e;
    for (i = 0; i < n; i++)
    {
        if (!q->pop(&e))
            return qeHandle->pop();
        else if (i == 0 && weight >= 0)
        {
            n = q->length();
            n >>= weight;
        }

        int overload = q->overload();
        if (overload)
        {
            ucontext::error(0, "May overload, event queue length %d", overload);
        }

        //if (ucontext->)
        ptr->execute(&e);
    }

    ptr->isAssert(q.get());
    eq_ptr nq = qeHandle->pop();
    if (nq)
    {
        qeHandle->push(q);
        q = nq;
    }

    return q;
}

void initLogger()
{
    ucontext *logCtx = new ucontext();
    if (logCtx->init(DEPLOY_H.logService, DEPLOY_H.logFile) == 0)
    {
        delete logCtx;
        logCtx = NULL;
        exit(1);
    }
}

void initBootstrap()
{
    if(DEPLOY_H.bootstrap == NULL) return;
    size_t sz = strlen(DEPLOY_H.bootstrap);
    char tmp[sz + 1];
    strcpy(tmp, DEPLOY_H.bootstrap);
    char *args = tmp;
    char *mod = strsep(&args, " \t\r\n");
    args = strsep(&args, "\r\n");

    ucontext *inst = new ucontext();
    if (inst->init(mod, args) == 0)
    {
        delete inst;
        inst = NULL;
        exit(1);
    }

}

bool loadDeploy(const char *filename)
{
    rednet::uini *hini = new rednet::uini();
    assert(hini);
    if (hini->load(filename) != 0)
    {
        fprintf(stderr, "Need a configuration file: load fail ------->>>> %s.\n", filename);
        return false;
    }

    rednet::uini::iterator it = hini->begin();
    while (it != hini->end())
    {
        ENV_CALL->setEnv(it->first.c_str(), it->second.c_str());
        ++it;
    }

    delete hini;
    return true;
}

void startup(const char *config_file)
{
    int i;
    udaemon *hdaemon = NULL;
    if (config_file == NULL)
    {
        fprintf(stderr, "Need a configuration file ------->>>> rednet [filename].\n");
        return;
    }

    SYSTEM_CALL->init();

    if (!loadDeploy(config_file))
    {
        goto _failed;
    }

    DEPLOY_H.workerNumber = ENV_CALL->__getInt("thread", 4);
    DEPLOY_H.serverGroupID = ENV_CALL->__getInt("server_group_id", 1);
    DEPLOY_H.daemon = ENV_CALL->__getString("daemon", NULL);
    DEPLOY_H.modulePath = ENV_CALL->__getString("module_path", "./service_cxx/?.so");
    DEPLOY_H.proFile = ENV_CALL->__getBoolean("profile", false);
    DEPLOY_H.bootstrap = ENV_CALL->__getString("bootstrap", "script bootstrap");
    DEPLOY_H.logService = ENV_CALL->__getString("logservice", NULL);
    DEPLOY_H.logFile = ENV_CALL->__getString("logfile", NULL);
    DEPLOY_H.luaPath = ENV_CALL->__getString("lua_path", "./lualib/?.lua;./lualib/?/init.lua");
    DEPLOY_H.luaCPath = ENV_CALL->__getString("lua_cpath", "./lib_cxx/?.so");
    DEPLOY_H.luaService = ENV_CALL->__getString("lua_serLOY_Hvice", "./script/?.lua");

    uglobal::instance()->setProFile(DEPLOY_H.proFile);
    REMOTE_CALL->init(DEPLOY_H.serverGroupID);
    UCTXS_CALL->init(DEPLOY_H.serverGroupID);
    UMODS_CALL->init(DEPLOY_H.modulePath);

    if (DEPLOY_H.daemon)
    {
        hdaemon = new udaemon(DEPLOY_H.daemon);
        if (hdaemon->init() != 0)
        {
            goto _failed;
        }
    }
    
    initLogger();
    initBootstrap();
    

    WORKS_CALL->append(std::bind(&rednet::usystem::workTimer, SYSTEM_CALL, std::placeholders::_1), NULL);
    for (i = 0; i < DEPLOY_H.workerNumber; i++)
    {
        WORKS_CALL->append(std::bind(&rednet::usystem::workLogic, SYSTEM_CALL, std::placeholders::_1), NULL);
    }

    WORKS_CALL->join();

    if (DEPLOY_H.daemon)
    {
        hdaemon->exit();
    }
_failed:
    SYSTEM_CALL->destory();
    if (hdaemon)
        delete hdaemon;
}

} // namespace rednet
