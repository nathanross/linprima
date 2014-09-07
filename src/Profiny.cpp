#ifdef PROFINY
#include "Profiny.h"
namespace profiny {
    boost::intrusive_ptr<Profiler> Profiler::m_instance = NULL;
}
#endif
