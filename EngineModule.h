#ifndef ENGINEMODULE_H_
#define ENGINEMODULE_H_

/* ====== INCLUDES ====== */
#include "Log.h"

/* ====== STRUCTURES ====== */
class EngineModule
{
    const char* m_moduleName;
    s32 m_moduleChannel;
protected:
    void SetModuleInfo(const char* name, s32 channel);
    void AddNote(s32 priority, const char* fmt, ...) const;
};

#endif // ENGINEMODULE_H_
