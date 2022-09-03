#ifndef ENGINEMODULE_H_
#define ENGINEMODULE_H_

/* ====== INCLUDES ====== */
#include "Types.h"
#include "DebugLogManager.h"

/* ====== STRUCTURES ====== */
class EngineModule
{
    const char* m_moduleName;
    s32 m_moduleChannel;
protected:
    void SetModuleInfo(const char* name, s32 channel);
    void AddNote(s32 priority, const char* fmt, ...) const;
public:
    const char* GetModuleName() const { return m_moduleName; }
    s32 GetModuleChannel() const { return m_moduleChannel; }
};

#endif // ENGINEMODULE_H_
