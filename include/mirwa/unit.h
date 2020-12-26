#ifndef MIRWA_UNIT_H
#define MIRWA_UNIT_H

typedef char MirwaUnit;
static const MirwaUnit mirwa_unit;

#ifndef MIRWA_ONLY_PREFIXED

typedef MirwaUnit Unit;
static const MirwaUnit unit;

#endif // MIRWA_ONLY_PREFIXED

#endif // MIRWA_UNIT_H
