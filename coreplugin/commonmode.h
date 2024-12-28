#ifndef COMMONMODE_H
#define COMMONMODE_H

#include <icontext.h>
#include "qsplitter.h"

class CommonMode : public IContext
{
    Q_OBJECT

public:
    CommonMode(QObject *parent = nullptr);
    ~CommonMode() override;

private:
    QSplitter *m_splitter;
};

#endif // COMMONMODE_H
