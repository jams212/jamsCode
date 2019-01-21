#ifndef REDNET_UENV_H
#define REDNET_UENV_H

#include "uobject.h"
#include "uspinlock.h"
#include <lua.hpp>

namespace rednet
{
class uenv : public uobject
{
public:
  uenv();
  ~uenv();

  const char *getEnv(const char *key);

  void setEnv(const char *key, const char *value);

public:
  static int __getInt(const char *key, int opt = 0);

  static bool __getBoolean(const char *key, bool opt = false);

  static const char *__getString(const char *key, const char *opt = NULL);

private:
  lua_State *l__;
  uspinlock lk__;
};

} // namespace rednet

#endif