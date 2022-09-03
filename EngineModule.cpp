/* ====== INCLUDES ====== */
#include "DebugLogManager.h"

#include "EngineModule.h"

/* ====== METHODS ====== */
void EngineModule::SetModuleInfo(const char* name, s32 channel)
{
    m_moduleName = name;
    m_moduleChannel = channel;
}

void EngineModule::AddNote(s32 priority, const char* fmt, ...) const
{
    va_list vl;
    va_start(vl, fmt);
    g_debugLogMgr.Note2(m_moduleChannel, priority, m_moduleName, fmt, vl);
    va_end(vl);
}