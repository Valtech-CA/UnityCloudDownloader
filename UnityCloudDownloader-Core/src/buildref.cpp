#include "buildref.h"

#include "build.h"
#include "builddao.h"
#include "servicelocator.h"

namespace ucd
{

BuildRef::BuildRef()
    : m_buildNumber(0)
{}

BuildRef::BuildRef(const Build &build)
    : m_buildTargetId(build.buildTargetId())
    , m_buildNumber(build.id())
{}

BuildRef &BuildRef::operator=(const Build &build)
{
    m_buildTargetId = build.buildTargetId();
    m_buildNumber = build.id();
    return *this;
}

bool BuildRef::operator==(const BuildRef &other) const
{
    return m_buildTargetId == other.m_buildTargetId && m_buildNumber == other.m_buildNumber;
}

bool BuildRef::operator<(const BuildRef &other) const
{
    if (m_buildTargetId < other.m_buildTargetId)
        return true;
    return m_buildNumber < other.m_buildNumber;
}

BuildRef::operator Build() const
{
    return BuildDao(ServiceLocator::database()).build(m_buildTargetId, m_buildNumber);

}

} // namespace ucd
