#include "QAlgorithmRegistry.h"

QAlgorithmRegistry &QAlgorithmRegistry::get_Instance()
{
    static QAlgorithmRegistry instance;
    return instance;
}
