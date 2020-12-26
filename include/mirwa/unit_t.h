#ifndef MIRWA_UNIT_T_H
#define MIRWA_UNIT_T_H

typedef char MirwaUnitT;
static const MirwaUnitT mirwa_unit;

#ifndef MIRWA_ONLY_PREFIXED

typedef MirwaUnitT UnitT;
static const MirwaUnitT unit;

#endif // MIRWA_ONLY_PREFIXED

#endif // MIRWA_UNIT_T_H
