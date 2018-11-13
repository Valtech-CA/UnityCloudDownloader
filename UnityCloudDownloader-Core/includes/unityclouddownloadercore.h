#ifndef UCD_CORE_H
#define UCD_CORE_H

namespace ucd
{

class Core
{
public:
    Core() = delete;
    Core(const Core&) = delete;
    ~Core() = delete;
    Core& operator=(const Core&) = delete;

    static void init();
};

}

#endif // UCD_CORE_H
